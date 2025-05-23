#ifndef MAIN_H
#define MAIN_H

#include "config.h"

#define THREAD1_STACK 0x100
#define THREAD2_STACK 0x800
#define THREAD3_STACK 0x200
#define THREAD4_STACK 0x2000
#define THREAD5_STACK 0x2000
#define THREAD6_STACK 0x400

struct RumbleData {
    u8  comm;
    u8  level;
    s16 time;
    s16 decay;
};

struct RumbleSettings {
    s16 event;
    s16 level;
    s16 timer;
    s16 count;
    s16 start;
    s16 slip;
    s16 vibrate;
    s16 decay;
};

extern struct Config gConfig;

extern OSThread D_80339210;
extern OSThread gIdleThread;
extern OSThread gMainThread;
extern OSThread gGameLoopThread;
extern OSThread gSoundThread;
extern OSThread hvqmThread;
#if ENABLE_RUMBLE
extern OSThread gRumblePakThread;

extern OSPfs gRumblePakPfs;
#endif

extern OSMesgQueue gPIMesgQueue;
extern OSMesgQueue gIntrMesgQueue;
extern OSMesgQueue gSPTaskMesgQueue;
#if ENABLE_RUMBLE
extern OSMesgQueue gRumblePakSchedulerMesgQueue;
extern OSMesgQueue gRumbleThreadVIMesgQueue;
#endif
extern OSMesg gDmaMesgBuf[1];
extern OSMesg gPIMesgBuf[32];
extern OSMesg gSIEventMesgBuf[1];
extern OSMesg gIntrMesgBuf[16];
extern OSMesg gUnknownMesgBuf[16];
extern OSIoMesg gDmaIoMesg;
extern OSMesg gMainReceivedMesg;
extern OSMesgQueue gDmaMesgQueue;
extern OSMesgQueue gSIEventMesgQueue;
#if ENABLE_RUMBLE
extern OSMesg gRumblePakSchedulerMesgBuf[1];
extern OSMesg gRumbleThreadVIMesgBuf[1];

extern struct RumbleData gRumbleDataQueue[3];
extern struct RumbleSettings gCurrRumbleSettings;
#endif

extern struct VblankHandler *gVblankHandler1;
extern struct VblankHandler *gVblankHandler2;
extern struct SPTask *gActiveSPTask;
extern u32 gNumVblanks;
extern s8 gResetTimer;
extern s8 gNmiResetBarsTimer;
extern s8 gDebugLevelSelect;
extern s8 gShowProfiler;
extern s8 gShowDebugText;


// Message IDs
#define MESG_SP_COMPLETE 100
#define MESG_DP_COMPLETE 101
#define MESG_VI_VBLANK 102
#define MESG_START_GFX_SPTASK 103
#define MESG_NMI_REQUEST 104
#define MESG_RCP_HUNG 105

typedef struct {
    u8 started;
    OSTimer timer;
} OSTimerEx;

extern u32 gHVQMPlaying;

void set_vblank_handler(s32 index, struct VblankHandler *handler, OSMesgQueue *queue, OSMesg *msg);
void dispatch_audio_sptask(struct SPTask *spTask);
void exec_display_list(struct SPTask *spTask);

#endif // MAIN_H
