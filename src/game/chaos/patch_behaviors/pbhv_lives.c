#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "sounds.h"
#include "audio/external.h"
#include "game/level_update.h"
#include "game/save_file.h"

#define LV1_LIVES_POS 2
#define LV2_LIVES_POS 4
#define LV3_LIVES_POS 6

#define LV1_LIVES_NEG 3
#define LV2_LIVES_NEG 6
#define LV3_LIVES_NEG 10

static u8 chs_cond_lives_check_negative_patch(void) {
    return (gNegativePatchCompare == CHAOS_PATCH_LIVES_DECREASE_LV1
                 || gNegativePatchCompare == CHAOS_PATCH_LIVES_DECREASE_LV2
                 || gNegativePatchCompare == CHAOS_PATCH_LIVES_DECREASE_LV3);
}

u8 chs_cond_lives_increase_lv1(void) { return (gChaosLivesEnabled && !chs_cond_lives_check_negative_patch() && gMarioState->numLives <= (100 - LV1_LIVES_POS)); }
u8 chs_cond_lives_increase_lv2(void) { return (gChaosLivesEnabled && !chs_cond_lives_check_negative_patch() && gMarioState->numLives <= (100 - LV2_LIVES_POS)); }
u8 chs_cond_lives_increase_lv3(void) { return (gChaosLivesEnabled && !chs_cond_lives_check_negative_patch() && gMarioState->numLives <= (100 - LV3_LIVES_POS)); }
u8 chs_cond_lives_decrease_lv1(void) { return (gChaosLivesEnabled && gMarioState->numLives >= LV1_LIVES_NEG); }
u8 chs_cond_lives_decrease_lv2(void) { return (gChaosLivesEnabled && gMarioState->numLives >= LV2_LIVES_NEG); }
u8 chs_cond_lives_decrease_lv3(void) { return (gChaosLivesEnabled && gMarioState->numLives >= LV3_LIVES_NEG); }

void chs_act_lives_increase_lv1(void) {
    gMarioState->numLives += LV1_LIVES_POS;
    play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives, TRUE);
}
void chs_act_lives_increase_lv2(void) {
    gMarioState->numLives += LV2_LIVES_POS;
    play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives, TRUE);
}
void chs_act_lives_increase_lv3(void) {
    gMarioState->numLives += LV3_LIVES_POS;
    play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives, TRUE);
}
void chs_act_lives_decrease_lv1(void) {
    gMarioState->numLives -= LV1_LIVES_NEG;
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives, TRUE);
}
void chs_act_lives_decrease_lv2(void) {
    gMarioState->numLives -= LV2_LIVES_NEG;
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives, TRUE);
}
void chs_act_lives_decrease_lv3(void) {
    gMarioState->numLives -= LV3_LIVES_NEG;
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives, TRUE);
}
