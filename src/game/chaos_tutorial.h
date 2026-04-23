#pragma once

#include "chaos_tutorial/chaos_tutorial_dma.h"

struct ChaosTutorialSlide {
    void *imageAddress;
    char *description;
};

enum ChaosTutorialState {
    CHS_TUT_STATE_STARTING,
    CHS_TUT_STATE_DEFAULT,
    CHS_TUT_STATE_ENDING,
};

extern s32 gChaosTutorialSlideIndex;
extern u8 *gChaosTutorialLoadedAddr;
extern u8 gChaosTutorialImgBuffer[CHAOS_TUTORIAL_IMG_SIZE];
extern struct ChaosTutorialSlide gChaosTutorialSlides[];
extern const s32 gChaosTutorialSlideCount;

void chstut_draw_shaded_background(Gfx** dl, s32 x1, s32 x2, s32 y1, s32 y2, u8 r, u8 g, u8 b, u8 a);
void chstut_tutorial_init(void);
void chstut_render_tutorial(void);
s32 chstut_update_tutorial(void);
