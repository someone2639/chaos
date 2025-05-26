#ifndef CHAOS_TUTORIAL_DMA_H
#define CHAOS_TUTORIAL_DMA_H

#include "types.h"

#define CHAOS_TUTORIAL_IMG_WIDTH 212
#define CHAOS_TUTORIAL_IMG_HEIGHT 160
#define CHAOS_TUTORIAL_IMG_SIZE ALIGN16(CHAOS_TUTORIAL_IMG_WIDTH * CHAOS_TUTORIAL_IMG_HEIGHT * 2)

extern u8 chsTutDMA_00_introduction[];
extern u8 chsTutDMA_01_select_patch[];
extern u8 chsTutDMA_02_selection_layout[];
extern u8 chsTutDMA_03_patch_card[];
extern u8 chsTutDMA_04_positive_negative[];
extern u8 chsTutDMA_05_patch_rank[];
extern u8 chsTutDMA_06_time_limit[];
extern u8 chsTutDMA_07_total_uses[];
extern u8 chsTutDMA_08_special_events[];
extern u8 chsTutDMA_09_event_lucky[];
extern u8 chsTutDMA_0A_event_bad_luck[];
extern u8 chsTutDMA_0B_event_chaos[];
extern u8 chsTutDMA_0C_star_duplicates[];
extern u8 chsTutDMA_0D_difficulty_selection[];
extern u8 chsTutDMA_0E_difficulty_easy[];
extern u8 chsTutDMA_0F_difficulty_hard[];
extern u8 chsTutDMA_10_challenge_mode[];
extern u8 chsTutDMA_11_conclusion[];

#endif // CHAOS_TUTORIAL_DMA_H
