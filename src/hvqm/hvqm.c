#include <ultra64.h>
#include <sm64.h>
#include <buffers/framebuffers.h>
#include <buffers/buffers.h>
#include <hvqm2dec.h>
#include <adpcmdec.h>
#include "hvqm.h"
#include "game/main.h"
#include "audio/data.h"

#define AUDIO_DMA_MSG_SIZE 1
static OSIoMesg     audioDmaMesgBlock;
static OSMesgQueue  audioDmaMessageQ;
static OSMesg       audioDmaMessages[AUDIO_DMA_MSG_SIZE];

/***********************************************************************
 *    Message queue for receiving message blocks and end of DMA 
 * notifications when requesting that video records be read from 
 * the HVQM2 data (ROM).
 ***********************************************************************/
#define  VIDEO_DMA_MSG_SIZE  1
static OSIoMesg     videoDmaMesgBlock;
static OSMesgQueue  videoDmaMessageQ;
static OSMesg       videoDmaMessages[VIDEO_DMA_MSG_SIZE];

/***********************************************************************
 * SP event (SP task end) message queue
 ***********************************************************************/
static OSMesgQueue  spMesgQ;
static OSMesg       spMesgBuf;

/***********************************************************************
 * RSP task data and parameter for the HVQM2 microcode
 ***********************************************************************/
OSTask hvqtask;			/* RSP task data */
HVQM2Arg hvq_sparg;		/* Parameter for the HVQM2 microcode */

/***********************************************************************
 * Buffer for the HVQM2 header ***********************************************************************/
u8 hvqm_headerBuf[sizeof(HVQM2Header) + 16];

/***********************************************************************
 * Other data
 ***********************************************************************/
static u32 total_frames;	/* Total number of video records (frames) */
static u32 total_audio_records;	/* Total number of audio records */
static void *video_streamP;	/* Video record read-in pointer */
static void *audio_streamP;	/* Audio record read-in pointer */
static u32 audio_remain;	/* Counter for remaining number of audio records to read */
static u32 video_remain;	/* Counter for remaining number of video records to read */
static u64 disptime;		/* Counter for scheduled display time of next video frame */
static ADPCMstate adpcm_state;	/* Buffer for state information passed to the ADPCM decoder */

/*
 * Macro for loading multi-byte data from buffer holding data from stream 
 */
#define load32(from) (*(u32*)&(from))
#define load16(from) (*(u16*)&(from))

void *gHVQM_VideoPointer;
static u64 hvqmStack[STACKSIZE / sizeof(u64)];

HVQM2Header *hvqm_header;

void hvqm_reset_bss(void) {
    total_frames = 0;
    total_audio_records = 0;
    video_streamP = NULL;
    audio_streamP = NULL;
    audio_remain = 0;
    video_remain = 0;
    disptime = 0;
    hvqm_header = 0;

    bzero(hvqm_headerBuf, sizeof(hvqm_headerBuf));
    bzero(&adpcm_state, sizeof(ADPCMstate));

    bzero(&audioDmaMesgBlock, sizeof(OSIoMesg));
    bzero(&audioDmaMessageQ, sizeof(OSMesgQueue));
    bzero(audioDmaMessages, sizeof(OSMesg));

    bzero(&videoDmaMesgBlock, sizeof(OSIoMesg));
    bzero(&videoDmaMessageQ, sizeof(OSMesgQueue));
    bzero(videoDmaMessages, sizeof(OSMesg));

    bzero(&spMesgQ, sizeof(OSMesgQueue));
    bzero(&spMesgBuf, sizeof(OSMesg));
    gHVQM_VideoPointer = NULL;
    bzero(&hvqtask, sizeof(OSTask));
    bzero(&hvq_sparg, sizeof(HVQM2Arg));

    bzero(hvqmStack, sizeof(hvqmStack));
    bzero(hvqwork, sizeof(u16) * (MAXWIDTH/8)*(MAXHEIGHT/4)*4);
    bzero(hvq_spfifo, HVQ_SPFIFO_SIZE);
    bzero(hvq_yieldbuf, HVQM2_YIELD_DATA_SIZE);

    init_cfb();
}

static u32 next_audio_record( void *pcmbuf ) {
  ALIGNED16 u8 header_buffer[sizeof(HVQM2Record)+16];
  HVQM2Record *record_header;
  HVQM2Audio *audio_headerP;
  u32 samples;

  if ( audio_remain == 0 ) return 0;

  record_header = OS_DCACHE_ROUNDUP_ADDR(header_buffer);
  audio_streamP = get_record(record_header, adpcmbuf, HVQM2_AUDIO, audio_streamP, &audioDmaMesgBlock, &audioDmaMessageQ);
  --audio_remain;

  audio_headerP = (HVQM2Audio *)adpcmbuf;
  samples = load32(audio_headerP->samples);
  adpcmDecode(&audio_headerP[1], (u32)load16(record_header->format), samples, pcmbuf, 1, &adpcm_state);

  return samples;
}

static tkAudioProc rewind( void ) {
  video_streamP = audio_streamP = (void*)((u32)gHVQM_VideoPointer + sizeof(HVQM2Header));
  audio_remain = total_audio_records;
  video_remain = total_frames;
  disptime = 0;
  return &next_audio_record;
}

HVQM2Header *hvqm_header;

static OSMesgQueue   hvqmMesgQ;
static OSMesg        hvqmMesgBuf;

OSThread hvqmThread;
static u64 hvqmStack[STACKSIZE/sizeof(u64)];

void hvqm_main_proc(uintptr_t vidPtr) {
    int h_offset = 0, v_offset = 0;	/* Position of image display */
    int screen_offset = 0;		/* Number of pixels from start of frame buffer to display position */
    u32 usec_per_frame;
    int prev_bufno = -1;

    
    gHVQM_VideoPointer = (void*)vidPtr;
    hvqm_header = OS_DCACHE_ROUNDUP_ADDR( hvqm_headerBuf );
    
    osCreateMesgQueue( &spMesgQ, &spMesgBuf, 1 );
    osSetEventMesg( OS_EVENT_SP, &spMesgQ, NULL );
    
    osCreateMesgQueue( &audioDmaMessageQ, audioDmaMessages, AUDIO_DMA_MSG_SIZE );
    osCreateMesgQueue( &videoDmaMessageQ, videoDmaMessages, VIDEO_DMA_MSG_SIZE );
    createTimekeeper();
    
    hvqm2InitSP1(0xff);
    hvqtask.t.ucode = (u64 *)hvqm2sp1TextStart;
    hvqtask.t.ucode_size = (int)hvqm2sp1TextEnd - (int)hvqm2sp1TextStart;
    hvqtask.t.ucode_data = (u64 *)hvqm2sp1DataStart;
    hvqtask.t.type = M_HVQM2TASK;
    hvqtask.t.flags = 0;
    hvqtask.t.ucode_boot = (u64 *)rspbootTextStart;
    hvqtask.t.ucode_boot_size = (int)rspbootTextEnd - (int)rspbootTextStart;
    hvqtask.t.ucode_data_size = HVQM2_UCODE_DATA_SIZE;
    hvqtask.t.data_ptr = (u64 *)&hvq_sparg;
    hvqtask.t.yield_data_ptr = (u64 *)hvq_yieldbuf;
    hvqtask.t.yield_data_size = HVQM2_YIELD_DATA_SIZE;

    init_cfb();
    osViSwapBuffer( gFramebuffers[NUM_CFBs-1] );

    romcpy(hvqm_header, (void *) vidPtr, sizeof(HVQM2Header), OS_MESG_PRI_NORMAL,
           &videoDmaMesgBlock, &videoDmaMessageQ);

    total_frames = load32(hvqm_header->total_frames);
    usec_per_frame = load32(hvqm_header->usec_per_frame);
    total_audio_records = load32(hvqm_header->total_audio_records);
    
    h_offset = (SCREEN_WD - hvqm_header->width) / 2;
    v_offset = (SCREEN_HT - hvqm_header->height) / 2;
    screen_offset = SCREEN_WD * v_offset + h_offset;
    
    hvqm2SetupSP1(hvqm_header, SCREEN_WD);
    
    release_all_cfb();
    tkStart( &rewind, load32( hvqm_header->samples_per_sec ) );
    
    for ( ; ; ) {

        while ( video_remain > 0 ) {
            u8 header_buffer[sizeof(HVQM2Record)+16];
            HVQM2Record *record_header;
            u16 frame_format;
            int bufno;
            // OSMesg msg;

            // char aa[50];
            // sprintf(aa, "Video Remain: %d\n", video_remain);
            // osSyncPrintf(aa);

            if ( disptime > 0 && tkGetTime() > 0) {
                if ( tkGetTime() < (disptime - (usec_per_frame * 2)) ) {
                   tkPushVideoframe( gFramebuffers[prev_bufno], &cfb_status[prev_bufno], disptime );
                   continue;
                  //if ( video_remain == 0 ) break;
                }
            }
            
            record_header = OS_DCACHE_ROUNDUP_ADDR( header_buffer );
            
            video_streamP = get_record(record_header, hvqbuf, 
                        HVQM2_VIDEO, video_streamP, 
                        &videoDmaMesgBlock, &videoDmaMessageQ);
                        
            //! SYNC VIDEO code

            if ( disptime > 0 && tkGetTime() > 0) {
                if ( tkGetTime() > (disptime + (usec_per_frame * 2)) ) {
                  release_all_cfb();
                  do {
                    disptime += usec_per_frame;
                    if ( --video_remain == 0 ) {
                        // osSyncPrintf("LINE 215 BREAK\n");
                        break;
                    }
                    video_streamP = get_record( record_header, hvqbuf, 
				    HVQM2_VIDEO, video_streamP, 
				    &videoDmaMesgBlock, &videoDmaMessageQ );
                  } while (load16( record_header->format ) != HVQM2_VIDEO_KEYFRAME || tkGetTime() > disptime );
                  if ( video_remain == 0 ) {
                      // osSyncPrintf("LINE 220 BREAK\n");
                      break;
                  }
                }
            }
            
            frame_format = load16(record_header->format);
            if (frame_format == HVQM2_VIDEO_HOLD) {
                /*
                 *   Just like when frame_format != HVQM2_VIDEO_HOLD you 
                     * could call hvqm2Decode*() and decode in a new frame
                     * buffer (in this case, just copying from the buffer of
                     * the preceding frame).  But here we make use of the
                     * preceding frame's buffer for the next frame in order
                     * to speed up the process.
                 */
                bufno = prev_bufno;
            } else {
                int status;
                bufno = get_cfb(); /* Get the frame buffer */

                /*
                 * Process first half in the CPU
                 */
                hvqtask.t.flags = 0;
                status = hvqm2DecodeSP1( hvqbuf, frame_format, 
                           &gFramebuffers[bufno][screen_offset], 
                           &gFramebuffers[prev_bufno][screen_offset], 
                           hvqwork, &hvq_sparg, hvq_spfifo );

                osWritebackDCacheAll();

                /*
                 * Process last half in the RSP
                 */
                if ( status > 0 ) {
                    osInvalDCache( (void *)gFramebuffers[bufno], sizeof gFramebuffers[bufno] );
                    osSpTaskStart( &hvqtask );
                    osRecvMesg( &spMesgQ, NULL, OS_MESG_BLOCK );
                }
            }
        
            keep_cfb( bufno );
            
            if ( prev_bufno >= 0 && prev_bufno != bufno ) 
              release_cfb( prev_bufno );

            tkPushVideoframe( gFramebuffers[bufno], &cfb_status[bufno], disptime );

            prev_bufno = bufno;
            disptime += usec_per_frame;
            --video_remain;
        }
        
        if (video_remain == 0) {
            // osSyncPrintf("Loop Broken\n");
            // osAiSetFrequency(gAudioSessionPresets.frequency);
            osSetEventMesg(OS_EVENT_AI, NULL, 0);
            osDestroyThread(&tkThread);
            osDestroyThread(&daCounterThread);
            osSendMesg(&gHVQM_SyncQueue, 0, OS_MESG_BLOCK);
            break;
        }
    }
}

void createHvqmThread(uintptr_t vidPtr) {
  osCreateMesgQueue( &hvqmMesgQ, &hvqmMesgBuf, 1 );
  osCreateThread( &hvqmThread, HVQM_THREAD_ID, (void*) hvqm_main_proc, 
		 (void*)vidPtr, hvqmStack + (STACKSIZE/sizeof(u64)), 
		 (OSPri)HVQM_PRIORITY );
}