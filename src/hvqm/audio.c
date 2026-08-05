#include <ultra64.h>
#include <HVQM2File.h>
#include <hvqm2dec.h>
#include <adpcmdec.h>
#include "macros.h"
#include "system.h"
#include "profiler.h"

int next_pcmbufno = 0;
int pcm_mod_samples = 0;
u32 samples_elapsed = 0;

#define AI_MSG_SIZE 2
static OSMesgQueue aiMessageQ;
static OSMesg aiMessages[AI_MSG_SIZE];

static ADPCMstate adpcm_state;

static void *audStreamPBase = 0;

typedef struct AudioRing {
    struct AudioRing *next;
    struct AudioRing *prev;
    s32 len;
    u64 endtime_us;
    s16 (*samples)[PCMBUF_SIZE];
} AudioRing;

extern volatile s32 hvqm_video_done;
volatile s32 hvqm_audio_done = FALSE;

AudioRing rbuffer[NUM_PCMBUFs] = {
    {.next = &rbuffer[1]},
    {.next = &rbuffer[2]},
    // {.next = &rbuffer[3]},
    // {.next = &rbuffer[4]},
    // {.next = &rbuffer[5]},
    // {.next = &rbuffer[6]},
    // {.next = &rbuffer[7]},
    // {.next = &rbuffer[8]},
    // {.next = &rbuffer[9]},
    // {.next = &rbuffer[10]},
    // {.next = &rbuffer[11]},
    // {.next = &rbuffer[12]},
    // {.next = &rbuffer[13]},
    // {.next = &rbuffer[14]},
    // {.next = &rbuffer[15]},
    {.next = &rbuffer[0]},
};

AudioRing *currBuf;

u64 playtime_us = 0;
u32 real_frequency = 0;
u32 num_channels = 1;

extern u64 disptime_us;

UNUSED static u32 audio_playing() {
    return (playtime_us != 0) && (disptime_us != 0) && (samples_elapsed > NUM_PCMBUFs);
}

static u32 samples2usec(AudioRing *buf) {
    return (((f32)buf->len / (f32)real_frequency) * 1000000.0f);
}

static s32 next_audio_record(void **streamp, void *pcmbuf, void *endDataAddr) {
    ALIGNED16 union {
        ALIGNED16 u8 _fill[ALIGN16(sizeof(HVQM2Record))];
        ALIGNED16 HVQM2Record record;
    } hdr;
    HVQM2Audio *audio_headerP;
    u32 samples;

    s32 size = get_record(&hdr.record, HVQM2_AUDIO, streamp, endDataAddr);
    if (size < 0) {
        hvqm_audio_done = TRUE;
        return -1;
    }
    if (load_record(size, HVQM2_AUDIO, adpcmbuf, streamp, endDataAddr)) {
        hvqm_audio_done = TRUE;
        return -1;
    }

    audio_headerP = (HVQM2Audio *) adpcmbuf;
    samples = load32(audio_headerP->samples);
    adpcmDecode(&audio_headerP[1], (u32) load16(hdr.record.format), samples, pcmbuf, 1,
                &adpcm_state);

    return samples;
}

static int ring_update(void **streamp, AudioRing *abuf, void *endDataAddr) {
    abuf->len = next_audio_record(streamp, abuf->samples, endDataAddr);
    if (abuf->len == -1) {
        return -1;
    }
    abuf->endtime_us = playtime_us + samples2usec(abuf)/2;
    return 0;
}

static void init_audio(void **streamp, void *endDataAddr) {
    // init ring buffer and load first N samples
    osCreateMesgQueue(&aiMessageQ, aiMessages, AI_MSG_SIZE);
    osSetEventMesg(OS_EVENT_AI, &aiMessageQ, (OSMesg *) 1);
    osSendMesg(&aiMessageQ, (OSMesg)0, OS_MESG_NOBLOCK);

    bzero(pcmbuf, sizeof(pcmbuf));

    for (int i = 0; i < NUM_PCMBUFs; i++) {
        rbuffer[(i + 1) % NUM_PCMBUFs].prev = &rbuffer[i];
        rbuffer[i].samples = &pcmbuf[i];
        int _ = ring_update(streamp, &rbuffer[i], endDataAddr);
        if (_ == -1) {
            hvqm_audio_done = TRUE;
            return;
        }
        playtime_us += samples2usec(&rbuffer[i]) / 2;
    }

    playtime_us = 0;
    currBuf = &rbuffer[0];
}

static u8 process_audio(void **streamp, void *endDataAddr) {
    u8 ret = TRUE;

    if (currBuf->len == -1) {
        hvqm_audio_done = TRUE;
        return TRUE;
    }

    osWritebackDCacheAll();

    osRecvMesg(&aiMessageQ, NULL, OS_MESG_BLOCK);
    int result = osAiSetNextBuffer(currBuf->samples, ALIGN(currBuf->len * 2 * sizeof(u16), 8));

    // if (playtime_us > currBuf->endtime_us) {
    if (result == 0) {
        currBuf = currBuf->next;
        samples_elapsed++;
        int _ = ring_update(streamp, currBuf->prev, endDataAddr);
        if (_ == -1) {
            hvqm_audio_done = TRUE;
        }
        ret = TRUE;
    } else {
        ret = FALSE;
    }

    playtime_us += samples2usec(currBuf);

    return ret;
}

UNUSED static void reset_audio(void **streamp, void *endDataAddr) {
    *streamp = audStreamPBase;
    osWritebackDCacheAll();
    init_audio(streamp, endDataAddr);
    playtime_us = 0;
    samples_elapsed = 0;
}

// UNUSED static void AudioImmediate(void **streamp) {
//     osRecvMesg(&aiMessageQ, NULL, OS_MESG_BLOCK);
//     s32 len = next_audio_record(streamp, pcmbuf[samples_elapsed % NUM_PCMBUFs]);
//     if (len == -1) {
//         audio_remain = 0;
//         return;
//     }

//     osWritebackDCacheAll();
//     int result = osAiSetNextBuffer(pcmbuf[samples_elapsed % NUM_PCMBUFs], ALIGN(len * 2 * sizeof(u16), 8));
//     if (result == 0) {
//         samples_elapsed++;
//         audio_remain--;
//     }
// }

void AudioMain(void *arg) {
    hvqmAudThreadParams *args = arg;
    void *streamp = audStreamPBase = args->streamp;
    num_channels = args->num_channels;
    // WARNING: If sample rate is lower than 32000, emulators will slow down!
    // TODO: Turn into an audio task using aspMain to resample all audio to 32k
    real_frequency = osAiSetFrequency(args->samples_per_sec);

    init_audio(&streamp, args->end_addr);
    playtime_us = 0;
    hvqm_audio_done = FALSE;

    

    while (1) {
        u8 wait = TRUE;
        while (!hvqm_audio_done && !hvqm_video_done) {
            wait = process_audio(&streamp, args->end_addr);
        }

        // osSyncPrintf("wait: %d\n", wait);

        if (wait) {
            osRecvMesg(&aiMessageQ, NULL, OS_MESG_BLOCK);
        }
        // osSyncPrintf("PLAYBACK DONE\n");

        extern OSThread hvqmThread;
        extern OSMesgQueue gHVQM_SyncQueue;
        extern OSMesgQueue viMessageQ;
        // get_fps_vals("HVQM Part1 (CPU)", "HVQM Part2 (RSP)");

        hvqm_audio_done = TRUE;

        // Lower priority to match video thread so that it can continue working while we yield
        osSetThreadPri(NULL, VID_PRIORITY);

        // NOTE: Don't wait for video if it takes longer than 1 second to finish after audio is done
        s32 time = osGetTime();
        while (!hvqm_video_done) {
            if (OS_CYCLES_TO_USEC(osGetTime() - time) > 1000000) {
                break;
            }
            osSyncPrintf("Waiting on HVQM video thread\n");
            osYieldThread();
        }

        // osSyncPrintf("audio pre-vi\n");

        // Wait 2 frames just in case
        osRecvMesg(&viMessageQ, NULL, OS_MESG_BLOCK);
        osRecvMesg(&viMessageQ, NULL, OS_MESG_BLOCK);

        // osSyncPrintf("audio teardown\n");
        osStopThread(&hvqmThread);
        osSetEventMesg(OS_EVENT_AI, NULL, 0);
        osSendMesg(&gHVQM_SyncQueue, (OSMesg*)0, OS_MESG_BLOCK);
        osDestroyThread(NULL);

#ifdef HVQM_VIDLOOP
        reset_audio(&streamp, args->end_addr);
#else
        break;
#endif // HVQM_VIDLOOP

    }

    while (1);
}
