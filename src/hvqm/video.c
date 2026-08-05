#include <ultra64.h>
#include <PR/os_internal_reg.h>
#include <HVQM2File.h>
#include <hvqm2dec.h>
#include "macros.h"
#include "system.h"
#include "profiler.h"
#include "buffers/framebuffers.h"
#include "game/debug.h"
#include "game/game_init.h"

OSMesgQueue spMesgQ;
static OSMesg spMesgBuf;

OSTask hvqtask;     // RSP task data
HVQM2Arg hvq_sparg; // Parameter for the HVQM2 microcode

extern volatile s32 hvqm_audio_done;
extern volatile s32 hvqm_video_done;
extern OSIoMesg dmaIOMesg;

typedef struct VideoRing {
    struct VideoRing *next;
    struct VideoRing *prev;
    int status;
    u16 format;
    CFBPix *cfb;
    CFBPix *drawbuf;
    u64 endtime_us;
} VideoRing;

int load_video_frame(void **streamp, VideoRing *vbuf, void *endDataAddr);
void decode_video(VideoRing *vbuf);
extern u64 playtime_us, disptime_us;
u64 disptime_us = 0;

VideoRing vbuffer[NUM_CFBs] = {
    {.next = &vbuffer[1], .prev = &vbuffer[NUM_CFBs - 1]},
    {.next = &vbuffer[2], .prev = &vbuffer[0]},
    {.next = &vbuffer[0], .prev = &vbuffer[1]},
};
VideoRing *currVBuf;

u32 usec_per_frame = 0;
u32 frames_elapsed = 0;

u32 video_playing() {
    return (playtime_us != 0) && (disptime_us != 0) && (frames_elapsed > NUM_CFBs);
}

void hvqm_reset_bss() {
    usec_per_frame = 0;
    frames_elapsed = 0;
    currVBuf = 0;
    playtime_us = disptime_us = 0;
}

void init_video(void **streamp, u32 offset, void *endDataAddr) {
    // new_profiler("HVQM Part1 (CPU)");
    // new_profiler("HVQM Part2 (RSP)");
    void hvqm_clearCurrentFB(void *buf, u32 size);

    for (int i = 0; i < NUM_CFBs; i++) {
        hvqm_clearCurrentFB(&gFramebuffers[i][0], sizeof(gFramebuffers[i]));
        vbuffer[i].cfb = &gFramebuffers[i][0];
        vbuffer[i].drawbuf = &gFramebuffers[i][offset];
        vbuffer[i].endtime_us = 0;
    }

    for (int i = 0; i < NUM_CFBs; i++) {
        int _ = load_video_frame(streamp, &vbuffer[i], endDataAddr);
        if (_ == -1) {
            hvqm_video_done = TRUE;
            return;
        }
        decode_video(&vbuffer[i]);
    }

    osViSwapBuffer(vbuffer[0].cfb);

    disptime_us = 0;
    currVBuf = &vbuffer[0];
}

void init_hvqm_task() {
    // Initialize the HVQM2 decoder
    // If using the RSP version of the decoder
    // also setup the RSP task data next
    hvqm2InitSP1(0xff);

    // Acquire an SP event (if using the RSP version of the decoder)
    osCreateMesgQueue(&spMesgQ, &spMesgBuf, 1);
    osSetEventMesg(OS_EVENT_SP, &spMesgQ, NULL);

    hvqtask.t.ucode = (u64 *) hvqm2sp1TextStart;
    hvqtask.t.ucode_size = (int) hvqm2sp1TextEnd - (int) hvqm2sp1TextStart;
    hvqtask.t.ucode_data = (u64 *) hvqm2sp1DataStart;
    hvqtask.t.type = M_HVQM2TASK;
    hvqtask.t.flags = 0;
    hvqtask.t.ucode_boot = (u64 *) rspbootTextStart;
    hvqtask.t.ucode_boot_size = (int) rspbootTextEnd - (int) rspbootTextStart;
    hvqtask.t.ucode_data_size = HVQM2_UCODE_DATA_SIZE;
    hvqtask.t.data_ptr = (u64 *) &hvq_sparg;
    hvqtask.t.yield_data_ptr = (u64 *) hvq_yieldbuf;
    hvqtask.t.yield_data_size = HVQM2_YIELD_DATA_SIZE;
}


// Loads the data required to decode a video frame
int load_video_frame(void **streamp, VideoRing *vbuf, void *endDataAddr) {
    ALIGNED16 union {
        ALIGNED16 u8 _fill[ALIGN16(sizeof(HVQM2Record))];
        ALIGNED16 HVQM2Record record;
    } hdr;
    // Get the next video record

    s32 record_size = get_record(&hdr.record, HVQM2_VIDEO, streamp, endDataAddr);
    if (record_size < 0) {
        hvqm_video_done = TRUE;
        return -1;
    }

    vbuf->format = load16(hdr.record.format);

    // A frame is scheduled for this many us past the start of the video
    u64 starttime_us = (frames_elapsed * usec_per_frame);

    if (starttime_us > usec_per_frame) {
        starttime_us -= usec_per_frame;
    }

    u32 skipped_frames = 0;

    // Frameskip
    if (video_playing()) {
        // Only skip if 2 audio frames behind
        if (playtime_us > (starttime_us + (usec_per_frame * 2))) {
            // Skip only as far as needed to sync up again, or to the next keyframe.
            //  Whichever comes first.
            while (playtime_us > starttime_us) {
                if (skip_record(record_size, &dmaIOMesg, streamp, endDataAddr)) {
                    return -1;
                }
                starttime_us += usec_per_frame;
                skipped_frames++;
                frames_elapsed++;
                record_size = get_record(&hdr.record, HVQM2_VIDEO, streamp, endDataAddr);
                if (record_size < 0) {
                    return -1;
                }
                if (hdr.record.format == HVQM2_VIDEO_KEYFRAME) {
                    // osSyncPrintf("(keyframed)\n");
                    // skup further if we're REALLY far behind
                    if (playtime_us > (starttime_us + (usec_per_frame * 2))) {
                        // osSyncPrintf("(we're REALLY far behind)\n");
                        continue;
                    } else {
                        break;
                    }
                }
            }
            // osSyncPrintf("(SKIPPED %d FRAMES)\n", skipped_frames);
        }
        if (hvqm_video_done) {
            return -1;
        } else {
            vbuf->format = load16(hdr.record.format);
        }
    }
    if (load_record(record_size, HVQM2_VIDEO, hvqbuf, streamp, endDataAddr)) {
        return -1;
    }

    vbuf->endtime_us = starttime_us;
    return 0;
}

// Actually decodes the frame
void decode_video(VideoRing *vbuf) {
    // Decode the compressed image data and expand it in the frame buffer
    if (vbuf->format == HVQM2_VIDEO_HOLD) {
       // do nothing
        vbuf->endtime_us += usec_per_frame;
    } else {
        // Process first half in the CPU
        hvqtask.t.flags = 0;

        // start_profiler("HVQM Part1 (CPU)");
        vbuf->status = hvqm2DecodeSP1(hvqbuf, vbuf->format, vbuf->drawbuf,
                                vbuf->prev->drawbuf, hvqwork,
                                &hvq_sparg, hvq_spfifo
                                );
        // end_profiler("HVQM Part1 (CPU)");
        // if (vbuf->format == HVQM2_VIDEO_KEYFRAME) {
        //     tag_profiler("HVQM Part1 (CPU)", "HVQM2_VIDEO_KEYFRAME");
        // } else {
        //     tag_profiler("HVQM Part1 (CPU)", "HVQM2_VIDEO_PREDICT");
        // }
        // print_profiler("HVQM Part1 (CPU)");
        osWritebackDCacheAll();

        // Process last half in the RSP
        if (vbuf->status > 0) {
            osInvalDCache((void *) vbuf->cfb, SCREEN_WD * SCREEN_HT * sizeof(CFBPix));
            // start_profiler("HVQM Part2 (RSP)");
            osSpTaskStart(&hvqtask);
            osRecvMesg(&spMesgQ, NULL, OS_MESG_BLOCK);
            // end_profiler("HVQM Part2 (RSP)");
            // print_profiler("HVQM Part2 (RSP)");
            
#ifdef DEBUG_ASSERTIONS
            const u32 saved = __osDisableInt();
            const u32 *addr = (u32*) hvq_spfifo;
            s32 index = HVQ_SPFIFO_SIZE * sizeof(HVQM2Info) / sizeof(u32) - 1;
            while (addr[index--] == 0);
            __osRestoreInt(saved);
            assert_args((f32) (index * sizeof(u32) / sizeof(HVQM2Info)) / (f32) HVQ_SPFIFO_SIZE < 0.8f, "HVQM decode_video:\nPlease increase hvq_spfifo buffer size!\n\nhvq_spfifo canary tripped at:\n  %.03f of total buffer usage", (f32) (index * sizeof(u32) / sizeof(HVQM2Info)) / (f32) HVQ_SPFIFO_SIZE);
#endif
        }
    }
}

void hold_all_frames(u32 count) {
    for (int i = 0; i < NUM_CFBs; i++) {
        vbuffer[i].endtime_us += (usec_per_frame * count);
    }
}

void show_next_frame(void **streamp, void *endDataAddr) {
    if (video_playing()) {
        // if (disptime_us > (playtime_us + (usec_per_frame * 10))) {
        //     u32 count = (disptime_us - playtime_us) / (usec_per_frame);
        //     // osSyncPrintf("HOLD %d\n", count);
        //     hold_all_frames(count);
        //     osYieldThread();
        // }
    }
    if (currVBuf->endtime_us <= playtime_us) {
        currVBuf = currVBuf->next;
        int _ = load_video_frame(streamp, currVBuf, endDataAddr);
        if (_ == -1) {
            hvqm_video_done = TRUE;
            return;
        }
        decode_video(currVBuf);
        frames_elapsed++;
    }
    if (currVBuf->format != HVQM2_VIDEO_HOLD) {
        if (gFBEEnabled == FALSE) {
            hvqm_drawHLE(currVBuf->cfb);
        } else {
            osViSwapBuffer(currVBuf->cfb);
        }
        osYieldThread();
    }
}

// Currently just a wrapper
void VideoMain(void **streamp, void *endDataAddr) {
    show_next_frame(streamp, endDataAddr);
}
