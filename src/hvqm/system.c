/*
 *  N64-HVQM2 library  Sample program
 *
 *  FILE : system.c (boot program/system utility)
 *
 *  Copyright (C) 1998,1999 NINTENDO Co.,Ltd.
 *
 */

/* 1998-12-15 */

#include <ultra64.h>
#include "system.h"
#include "game/game_init.h"

/***********************************************************************
 * Main thread
 ***********************************************************************/
OSThread hvqmThread;
static u64 hvqmThreadStack[STACKSIZE / 8];

#define video_glistp gDisplayListHead

extern OSMesgQueue spMesgQ;

static void render_multi_image(Texture *image, s32 x, s32 y, s32 width, s32 height) {
    s32 posW, posH, imW, imH;
    s32 i = 0;
    s32 num = 256;
    s32 maskW = 1;
    s32 maskH = 1;

    gDPSetCycleType(video_glistp++, G_CYC_COPY);
    gDPSetRenderMode(video_glistp++, G_RM_NOOP, G_RM_NOOP2);

    // Find how best to seperate the horizontal. Keep going until it finds a whole value.
    while (TRUE) {
        f32 val = (f32) width / (f32) num;

        if ((s32) val == val && (s32) val >= 1) {
            imW = num;
            break;
        }
        num /= 2;
        if (num == 1) {
            return;
        }
    }
    // Find the tile height
    imH = 64 / (imW / 32); // This gets the vertical amount.

    num = 2;
    // Find the width mask
    while (TRUE) {
        if ((s32) num == imW) {
            break;
        }
        num *= 2;
        maskW++;
        if (maskW == 9) {
            return;
        }
    }
    num = 2;
    // Find the height mask
    while (TRUE) {
        if ((s32) num == imH) {
            break;
        }
        num *= 2;
        maskH++;
        if (maskH == 9) {
            return;
        }
    }
    num = height;
    // Find the height remainder
    s32 peakH = height - (height % imH);
    s32 cycles = (width * peakH) / (imW * imH);

    // Pass 1
    for (i = 0; i < cycles; i++) {
        posW = 0;
        posH = i * imH;
        while (posH >= peakH) {
            posW += imW;
            posH -= peakH;
        }

        gDPLoadSync(video_glistp++);
        gDPLoadTextureTile(video_glistp++, image, G_IM_FMT_RGBA, G_IM_SIZ_16b, width, height, posW,
                           posH, ((posW + imW) - 1), ((posH + imH) - 1), 0, (G_TX_NOMIRROR | G_TX_WRAP),
                           (G_TX_NOMIRROR | G_TX_WRAP), maskW, maskH, 0, 0);
        gSPScisTextureRectangle(video_glistp++, ((x + posW) << 2), ((y + posH) << 2),
                                (((x + posW + imW) - 1) << 2), (((y + posH + imH) - 1) << 2),
                                G_TX_RENDERTILE, 0, 0, (4 << 10), (1 << 10));
    }
    // If there's a remainder on the vertical side, then it will cycle through that too.
    if (height - peakH != 0) {
        posW = 0;
        posH = peakH;
        for (i = 0; i < (width / imW); i++) {
            posW = i * imW;
            gDPLoadSync(video_glistp++);
            gDPLoadTextureTile(video_glistp++, image, G_IM_FMT_RGBA, G_IM_SIZ_16b, width, height, posW,
                               posH, ((posW + imW) - 1), (height - 1), 0, (G_TX_NOMIRROR | G_TX_WRAP),
                               (G_TX_NOMIRROR | G_TX_WRAP), maskW, maskH, 0, 0);
            gSPScisTextureRectangle(video_glistp++, (x + posW) << 2, (y + posH) << 2,
                                    ((x + posW + imW) - 1) << 2, ((y + posH + imH) - 1) << 2,
                                    G_TX_RENDERTILE, 0, 0, (4 << 10), (1 << 10));
        }
    }
}

void create_gfx_task_structure();

OSMesgQueue dpMesgQ;
OSMesg dpMesg;

void hvqm_drawHLE(void *buf) {
    select_gfx_pool();
    // gDPPipeSync(video_glistp++);
    gDPSetColorImage(video_glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, buf);
    gDPSetScissor(video_glistp++, G_SC_NON_INTERLACE, 0, 0, 320, 240);

    render_multi_image((Texture *) buf, 0, 0, 320, 240);

    gDPFullSync(video_glistp++);
    gSPEndDisplayList(video_glistp++);

    osWritebackDCacheAll();
    create_gfx_task_structure();
    gGfxSPTask->task.t.data_ptr = (u64 *) gGfxPool->buffer;
    gGfxSPTask->task.t.data_size = ((u32)gDisplayListHead - (u32)gGfxPool->buffer) * sizeof(Gfx);
    osSpTaskStart(&gGfxSPTask->task);
    osRecvMesg(&spMesgQ, NULL, OS_MESG_BLOCK);
    osRecvMesg(&dpMesgQ, NULL, OS_MESG_BLOCK);
}

static void mainproc(void *arg) {
    osCreateMesgQueue(&dpMesgQ, &dpMesg, 1);
    osSetEventMesg(OS_EVENT_DP, &dpMesgQ, (OSMesg*)1);
    /* To main function */
    Main(arg);

    /* To idle state */
    osSetThreadPri(0, 0);
    for (;;)
        ;
    /* NOT REACHED */
}

void createHvqmThread(void *arg) {
    bzero(&hvqmThread, sizeof(OSThread));
    osCreateThread(&hvqmThread, 7, mainproc, arg, hvqmThreadStack + STACKSIZE / 8, 11);
}

