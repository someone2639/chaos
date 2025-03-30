#include "sm64.h"

#include "chaos_tutorial.h"
#include "debug.h"
#include "game_init.h"
#include "memory.h"

static const Gfx dl_chstut_img_1cycle_begin[] = {
    gsDPPipeSync(),
    gsDPSetTexturePersp(G_TP_NONE),
    gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0),
    gsDPSetTextureFilter(G_TF_POINT),
    gsSPEndDisplayList(),
};

static const Gfx dl_chstut_img_1cycle_end[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsSPEndDisplayList(),
};

static void chstut_render_tiled_image(Gfx **dl, Texture *image, s32 x, s32 y, s32 width, s32 height, s32 useCopyMode) {
    Gfx *dlHead = *dl;

    s32 modeSC;
    s32 mOne;
    s32 maxWidth = 64;
    s32 maxHeight = 32;

    if (height > width) {
        maxHeight = 32;
        maxWidth = 64;
    }

    gSPDisplayList(dlHead++, dl_chstut_img_1cycle_begin);
    if (useCopyMode) {
        gDPSetCycleType(dlHead++, G_CYC_COPY);
        gDPSetRenderMode(dlHead++, G_RM_NOOP, G_RM_NOOP2);
        modeSC = 4;
        mOne = 1;
    } else {
        gDPSetCycleType(dlHead++, G_CYC_1CYCLE);
        gDPSetRenderMode(dlHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        modeSC = 1;
        mOne = 0;
    }

    for (s32 curImgY = 0; curImgY < height; curImgY += maxHeight) {
        s32 tileHeight = MIN(height - curImgY, maxHeight);
        s32 heightMask = 0;
        for (s32 val = tileHeight - 1; val > 0; val >>= 1) {
            heightMask++;
        }

        for (s32 curImgX = 0; curImgX < width; curImgX += maxWidth) { 
            s32 tileWidth = MIN(width - curImgX, maxWidth);
            s32 widthMask = 0;
            for (s32 val = tileWidth - 1; val > 0; val >>= 1) {
                widthMask++;
            }

            gDPLoadSync(dlHead++);
            gDPLoadTextureTile(dlHead++,
                image, G_IM_FMT_RGBA, G_IM_SIZ_16b, width, height, curImgX, curImgY, ((curImgX + tileWidth) - 1), ((curImgY + tileHeight) - 1), 0, (G_TX_NOMIRROR | G_TX_WRAP), (G_TX_NOMIRROR | G_TX_WRAP), widthMask, heightMask, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(dlHead++,
                ((x + curImgX) << 2),
                ((y + curImgY) << 2),
                (((x + curImgX + tileWidth) - mOne) << 2),
                (((y + curImgY + tileHeight) - mOne) << 2),
                G_TX_RENDERTILE, 0, 0, (modeSC << 10), (1 << 10));
        }
    }

    gSPDisplayList(dlHead++, dl_chstut_img_1cycle_end);

    *dl = dlHead;
}

void chstut_load_image(u8 *imgAddress) {
    if (imgAddress == gChaosTutorialLoadedAddr) {
        return;
    }
    gChaosTutorialLoadedAddr = imgAddress;

    if (imgAddress == NULL) {
        bzero(gChaosTutorialImgBuffer, sizeof(gChaosTutorialImgBuffer));
        return;
    }

    dma_read(gChaosTutorialImgBuffer, (u8 *) imgAddress, (u8 *) imgAddress + (size_t) CHAOS_TUTORIAL_IMG_SIZE);
}

void chstut_render_image(u8 r, u8 g, u8 b, u8 a) {
    if (gChaosTutorialLoadedAddr == NULL) {
        return;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, r, g, b, a);

    chstut_render_tiled_image(&gDisplayListHead, gChaosTutorialImgBuffer, SCREEN_CENTER_X - ((CHAOS_TUTORIAL_IMG_WIDTH + 1) / 2), 12, CHAOS_TUTORIAL_IMG_WIDTH, CHAOS_TUTORIAL_IMG_HEIGHT, (a == 255) ? TRUE : FALSE);

    gDPSetPrimColor(gDisplayListHead++, 0, 0, 255, 255, 255, 255);
}
