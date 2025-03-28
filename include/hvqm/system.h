/* 
 *  N64-HVQM2 library  Sample program
 * 
 *  FILE : system.h
 * 
 *  Copyright (C) 1998,1999 NINTENDO Co.,Ltd.
 * 
 */

/* 1999-04-12 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "macros.h"

#define STACKSIZE  0x2000

/**********************************************************************/

/* Kind of HQM2 decoder to use */
#ifndef USE_RSP
#define USE_RSP 1   /* 0: Use CPU decoder, 1: use RSP decoder */
#endif

/* Whether to forcedly synchronize video to audio when video playback is late */
#ifndef SYNC_VIDEO
#define SYNC_VIDEO 1    /* 0: Do not force synchronization, 1: Force synchronization */
#endif

/**********************************************************************/

#ifdef _LANGUAGE_C

#include <ultra64.h>
#include <HVQM2File.h>
#include <hvqm2dec.h>

#define ALIGNED(x) __attribute__((aligned(x)))

/*
 * Size of buffer for video records 
 */
#define HVQ_DATASIZE_MAX  300000

/*
 * Size of buffer for audio records
 */
#define AUDIO_RECORD_SIZE_MAX  30000

/*
 * Size of data area for HVQM2 microcode
 */
#define HVQ_SPFIFO_SIZE   200000

/*
 * Frame buffer specifications
 */
#define NUM_CFBs    3   /* Number of frame buffers (2 or more; at least 3 recommended) */

#define SCREEN_WD   320 /* Screen width [pixel] */
#define SCREEN_HT   240 /* Screen height [pixel] */

#ifndef CFB_FORMAT
#define CFB_FORMAT  1   /* Frame buffer format, 1: 16bit, 2: 32bit */
#endif

typedef u16 CFBPix;
#define VIMODE  OS_VI_NTSC_LAN1

#define VIFEAT  (OS_VI_DIVOT_OFF | OS_VI_GAMMA_ON)

/*
 * Frame buffer state flag 
 */
#define CFB_FREE     0      /* Available */
#define CFB_PRECIOUS (1<<0) /* Constrained for decoding of next frame */
#define CFB_SHOWING  (1<<1) /* Waiting to display or displaying */

/*
 * Audio DA specifications
 */
#define  PCM_CHANNELS        2  /* Number of channels */
#define  PCM_CHANNELS_SHIFT  1  /* log2(PCM_CHANNELS) */
#define  PCM_ALIGN           2  /* Alignment of number of samples to send */
#define  PCM_BYTES_PER_SAMPLE  (2 * PCM_CHANNELS) /* Number of bytes in one sample */
#define  PCM_BYTES_PER_SAMPLE_SHIFT  2  /* log2(PCM_BYTES_PER_SAMPLE) */

/*
 * Audio record definitions
 */
#define  AUDIO_SAMPLE_BITS  4
#define  AUDIO_SAMPLES_MAX  (((AUDIO_RECORD_SIZE_MAX-sizeof(HVQM2Audio))*8/AUDIO_SAMPLE_BITS)+1) /* Maximum number of records per sample */

/*
 * PCM buffer specifications
 */
#define  NUM_PCMBUFs    2  /* Number of PCM buffers (2 or more, at least 3 recommended) */
#define  PCMBUF_SIZE     0x2000

/*
 * Macro for loading multi-byte data from buffer holding data from stream 
 */
#define load32(from) (*(u32*)&(from))
#define load16(from) (*(u16*)&(from))

/*
 * Thread ID and priority
 */
enum Threads {
    IDLE_THREAD_ID = 1,
    MAIN_THREAD_ID,
    AUD_THREAD_ID,
    VIDEO_THREAD_ID,
    FAULT_THREAD_ID,
    SCHED_THREAD_ID,
    CONT_THREAD_ID,
};

#define IDLE_PRIORITY         10
#define MAIN_PRIORITY         10
#define VID_PRIORITY          14
#define AUD_PRIORITY          14
#define CONT_PRIORITY         12

#define PI_COMMAND_QUEUE_SIZE   8

/*
 * in main.c
 */
void Main(void *);

/*
 * in memory.c
 */
void romcpy(void *dest, void *src, u32 len, s32 pri, OSIoMesg *mb, OSMesgQueue *mq);
void dma_copy(void *dest, void *src, u32 len, OSIoMesg *msg);
u32 get_record(HVQM2Record *headerbuf, u16 type, void **stream);
void load_record(u32 record_size, u16 type, void *bodybuf, void **streamp);
void skip_record(u32 record_size, void **streamp);

typedef struct {
    void *streamp; // ptr to first aud record
    u32 remain; // remaining audio frames
    u32 samples_per_sec;
    u32 num_channels;
} AudThreadParams;

extern HVQM2Header hvqm_header;

// Buffers
extern u16 hvqwork[(MAXWIDTH/8)*(MAXHEIGHT/4)*4];       /* Work buffer for HVQM2 decoder */
extern u64 hvq_yieldbuf[];  /* RSP task yield buffer */
extern HVQM2Info hvq_spfifo[];  /* Data area for HVQM2 microcode */
extern u8 adpcmbuf[];       /* Buffer for audio records ADPCM) */
extern u8 hvqbuf[];     /* Buffer for video records (HVQM2) */
extern s16 pcmbuf[NUM_PCMBUFs][PCMBUF_SIZE]; /* PCM data buffer */
extern CFBPix cfb[NUM_CFBs][SCREEN_WD*SCREEN_HT]; /* Image frame buffer */

extern void init_dma();
extern void init_hvqm_task();
extern void init_video(void **, u32);
extern void show_next_frame(void **);

// fault
extern void crash_screen_init();

#define UNUSED __attribute__((unused))

/*
 *  Address of HVQM2 data
 */
// extern char _hvqmdataSegmentRomStart[], _hvqmdataSegmentRomEnd[];

#endif /* _LANGUAGE_C */

#endif /* __SYSTEM_H__ */

/* end */
