#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "sounds.h"
#include "audio/external.h"
#include "game/level_update.h"

#define LV1_LIVES 3
#define LV2_LIVES 6
#define LV3_LIVES 10

u8 chs_cond_lives_increase_lv1(UNUSED const struct ChaosPatch *patch) { return gMarioState->numLives <= (100 - LV1_LIVES); }
u8 chs_cond_lives_increase_lv2(UNUSED const struct ChaosPatch *patch) { return gMarioState->numLives <= (100 - LV2_LIVES); }
u8 chs_cond_lives_increase_lv3(UNUSED const struct ChaosPatch *patch) { return gMarioState->numLives <= (100 - LV3_LIVES); }
u8 chs_cond_lives_decrease_lv1(UNUSED const struct ChaosPatch *patch) { return gMarioState->numLives >= LV1_LIVES; }
u8 chs_cond_lives_decrease_lv2(UNUSED const struct ChaosPatch *patch) { return gMarioState->numLives >= LV2_LIVES; }
u8 chs_cond_lives_decrease_lv3(UNUSED const struct ChaosPatch *patch) { return gMarioState->numLives >= LV3_LIVES; }

void chs_act_lives_increase_lv1(UNUSED const struct ChaosPatch *patch) {
    gMarioState->numLives += LV1_LIVES;
    play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
}
void chs_act_lives_increase_lv2(UNUSED const struct ChaosPatch *patch) {
    gMarioState->numLives += LV2_LIVES;
    play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
}
void chs_act_lives_increase_lv3(UNUSED const struct ChaosPatch *patch) {
    gMarioState->numLives += LV3_LIVES;
    play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
}
void chs_act_lives_decrease_lv1(UNUSED const struct ChaosPatch *patch) {
    gMarioState->numLives -= LV1_LIVES;
}
void chs_act_lives_decrease_lv2(UNUSED const struct ChaosPatch *patch) {
    gMarioState->numLives -= LV2_LIVES;
}
void chs_act_lives_decrease_lv3(UNUSED const struct ChaosPatch *patch) {
    gMarioState->numLives -= LV3_LIVES;
}
