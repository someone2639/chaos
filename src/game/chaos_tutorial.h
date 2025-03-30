#pragma once

#include "chaos_tutorial/chaos_tutorial_dma.h"

struct ChaosTutorialSlide {
    void *imageAddress;
    char *description;
};

extern u8 *gChaosTutorialLoadedAddr;
extern u8 gChaosTutorialImgBuffer[CHAOS_TUTORIAL_IMG_SIZE];
extern struct ChaosTutorialSlide gChaosTutorialSlides[];
extern const u32 gChaosTutorialSlideCount;

void chstut_load_image(u8 *imgAddress);
void chstut_render_image(u8 r, u8 g, u8 b, u8 a);
