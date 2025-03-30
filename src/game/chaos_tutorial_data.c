#include "chaos_tutorial.h"

u8 *gChaosTutorialLoadedAddr = NULL;
u8 gChaosTutorialImgBuffer[CHAOS_TUTORIAL_IMG_SIZE];

struct ChaosTutorialSlide gChaosTutorialSlides[] = {
    {
        .imageAddress = chsTutDMA_00_introduction,
        .description = "Why hello there!",
    },
    {
        .imageAddress = chsTutDMA_01_select_patch,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_02_selection_layout,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_03_patch_card,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_04_positive_negative,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_05_patch_rank,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_06_time_limit,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_07_total_uses,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_08_event_lucky,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_09_event_bad_luck,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_0A_event_chaos,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_0B_difficulty_easy,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_0C_difficulty_hard,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_0D_challenge_mode,
        .description = "",
    },
    {
        .imageAddress = chsTutDMA_0E_conclusion,
        .description = "",
    },
};

const u32 gChaosTutorialSlideCount = ARRAY_COUNT(gChaosTutorialSlides);
