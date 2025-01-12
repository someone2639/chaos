#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "sounds.h"
#include "audio/external.h"
#include "game/level_update.h"
#include "game/save_file.h"

#define LV1_LIVES 3
#define LV2_LIVES 6
#define LV3_LIVES 10

u8 chs_cond_lives_increase_lv1(void) { return (gChaosLivesEnabled && gMarioState->numLives <= (100 - LV1_LIVES)); }
u8 chs_cond_lives_increase_lv2(void) { return (gChaosLivesEnabled && gMarioState->numLives <= (100 - LV2_LIVES)); }
u8 chs_cond_lives_increase_lv3(void) { return (gChaosLivesEnabled && gMarioState->numLives <= (100 - LV3_LIVES)); }
u8 chs_cond_lives_decrease_lv1(void) { return (gChaosLivesEnabled && gMarioState->numLives >= LV1_LIVES); }
u8 chs_cond_lives_decrease_lv2(void) { return (gChaosLivesEnabled && gMarioState->numLives >= LV2_LIVES); }
u8 chs_cond_lives_decrease_lv3(void) { return (gChaosLivesEnabled && gMarioState->numLives >= LV3_LIVES); }

void chs_act_lives_increase_lv1(void) {
    gMarioState->numLives += LV1_LIVES;
    play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives);
}
void chs_act_lives_increase_lv2(void) {
    gMarioState->numLives += LV2_LIVES;
    play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives);
}
void chs_act_lives_increase_lv3(void) {
    gMarioState->numLives += LV3_LIVES;
    play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives);
}
void chs_act_lives_decrease_lv1(void) {
    gMarioState->numLives -= LV1_LIVES;
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives);
}
void chs_act_lives_decrease_lv2(void) {
    gMarioState->numLives -= LV2_LIVES;
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives);
}
void chs_act_lives_decrease_lv3(void) {
    gMarioState->numLives -= LV3_LIVES;
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives);
}
