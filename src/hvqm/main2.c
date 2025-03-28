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

static OSThread audThread;
static u64 audThreadStack[STACKSIZE / 8];

extern void AudioMain(void *arg);
extern void VideoMain(void *arg);

extern u64 disptime_us;

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
    extern u32 video_remain;
    video_remain = total_frames - NUM_CFBs;

    void *audio_streamP = video + sizeof(HVQM2Header);
    u32 audio_remain = total_audio_records;

    AudThreadParams parms;
    if (total_audio_records != 0) {
        parms.streamp = audio_streamP;
        parms.remain = audio_remain;
        parms.samples_per_sec = hvqm_header.samples_per_sec;
        parms.num_channels = hvqm_header.channels;
        bzero(&audThread, sizeof(OSThread));
        osCreateThread(&audThread, 8, AudioMain, &parms, audThreadStack + STACKSIZE / 8,
                       AUD_PRIORITY);
        osStartThread(&audThread);
    }

    h_offset = (SCREEN_WD - hvqm_header.width) / 2;
    v_offset = (SCREEN_HT - hvqm_header.height) / 2;
    screen_offset = SCREEN_WD * v_offset + h_offset;

    // Setup the HVQM2 image decoder
    hvqm2SetupSP1(&hvqm_header, SCREEN_WD);
    init_video(&video_streamP, screen_offset);
    extern OSMesgQueue gHVQM_SyncQueue;

    osSetTime(0);
    while (video_remain > 0) {
        VideoMain(&video_streamP);


        disptime_us = OS_CYCLES_TO_USEC(osGetTime());
        osRecvMesg(&viMessageQ, NULL, OS_MESG_BLOCK);
    }
    osSetEventMesg(OS_EVENT_AI, NULL, 0);
    osJamMesg(&gHVQM_SyncQueue, (OSMesg*)0, OS_MESG_BLOCK);
    osStopThread(NULL);
}