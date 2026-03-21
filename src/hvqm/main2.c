#include <ultra64.h>
#include <HVQM2File.h>
#include <hvqm2dec.h>
#include <adpcmdec.h>
#include "buffers/framebuffers.h"
#include "system.h"

#define VI_MSG_SIZE 2
OSMesgQueue viMessageQ;
static OSMesg viMessages[VI_MSG_SIZE];

HVQM2Header hvqm_header __attribute__((aligned(16)));

OSThread hvqmAudThread;
static u64 hvqmAudThreadStack[STACKSIZE / 8];

extern void AudioMain(void *arg);
extern void VideoMain(void *arg);

extern u64 disptime_us;
volatile s32 hvqm_video_done = FALSE;
extern volatile s32 hvqm_audio_done;

void Main(void *video) {
    int h_offset, v_offset; // Position of image display
    int screen_offset;      // Number of pixels from start of frame buffer to display position

    // Acquire retrace event
    osCreateMesgQueue(&viMessageQ, viMessages, VI_MSG_SIZE);
    osViSetEvent(&viMessageQ, 0, 1);

    init_dma();
    init_hvqm_task();

    // Initialize the frame buffer (clear buffer contents and status flag)
    osViSwapBuffer(gFramebuffers[NUM_CFBs - 1]);

    // Fetch the HVQM2 header
    dma_copy(&hvqm_header, video, sizeof(HVQM2Header), NULL);

    u32 total_frames = load32(hvqm_header.total_frames);
    extern u32 usec_per_frame;
    usec_per_frame = load32(hvqm_header.usec_per_frame);
    u32 total_audio_records = load32(hvqm_header.total_audio_records);

    void *video_streamP = video + sizeof(HVQM2Header);
    extern volatile s32 video_remain;
    video_remain = total_frames - NUM_CFBs;
    hvqm_audio_done = FALSE;
    hvqm_video_done = FALSE;

    void *audio_streamP = video + sizeof(HVQM2Header);
    s32 audio_remain = total_audio_records;

    osSetTime(0);

    hvqmAudThreadParams parms;
    s32 startedAudioThread = FALSE;

    if (total_audio_records != 0) {
        startedAudioThread = TRUE;
        parms.streamp = audio_streamP;
        parms.remain = audio_remain;
        parms.samples_per_sec = hvqm_header.samples_per_sec;
        parms.num_channels = hvqm_header.channels;
        bzero(&hvqmAudThread, sizeof(OSThread));
        osCreateThread(&hvqmAudThread, 8, AudioMain, &parms, hvqmAudThreadStack + STACKSIZE / 8,
                       AUD_PRIORITY);
        osStartThread(&hvqmAudThread);
    }

    h_offset = (SCREEN_WD - hvqm_header.width) / 2;
    v_offset = (SCREEN_HT - hvqm_header.height) / 2;
    screen_offset = SCREEN_WD * v_offset + h_offset;

    // Setup the HVQM2 image decoder
    hvqm2SetupSP1(&hvqm_header, SCREEN_WD);
    init_video(&video_streamP, screen_offset);
    extern OSMesgQueue gHVQM_SyncQueue;

    while (video_remain > 0 && !hvqm_audio_done) {
        VideoMain(&video_streamP);

        char t[50];
        sprintf(t, "video_remain: %d\n", video_remain);
        osSyncPrintf(t);

        disptime_us = OS_CYCLES_TO_USEC(osGetTime());
        osRecvMesg(&viMessageQ, NULL, OS_MESG_BLOCK);
    }

    hvqm_video_done = TRUE;

    if (startedAudioThread) {
        // Allow audio thread to stop the main HQVM thread
        while(1);
    }

    // Wait 2 frames just in case
    osRecvMesg(&viMessageQ, NULL, OS_MESG_BLOCK);
    osRecvMesg(&viMessageQ, NULL, OS_MESG_BLOCK);

    osSyncPrintf("before handback...");
    osSetEventMesg(OS_EVENT_AI, NULL, 0);
    osSendMesg(&gHVQM_SyncQueue, (OSMesg*)0, OS_MESG_BLOCK);
    osStopThread(NULL);
}