#pragma once

#include "chaos_tutorial/chaos_tutorial_dma.h"

struct ChaosTutorialSlide {
    void *imageAddress;
    char *description;
};

extern s32 gChaosTutorialSlideIndex;
extern u8 *gChaosTutorialLoadedAddr;
extern u8 gChaosTutorialImgBuffer[CHAOS_TUTORIAL_IMG_SIZE];
extern struct ChaosTutorialSlide gChaosTutorialSlides[];
extern const s32 gChaosTutorialSlideCount;

void chstut_tutorial_init(void);
void chstut_render_tutorial(void);
