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

#define LV1_LIVES_NEG 4
#define LV2_LIVES_NEG 8
#define LV3_LIVES_NEG 12

u8 chs_cond_lives_increase_lv1(void) { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE && gMarioState->numLives <= (100 - LV1_LIVES_POS)); }
u8 chs_cond_lives_increase_lv2(void) { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE && gMarioState->numLives <= (100 - LV2_LIVES_POS)); }
u8 chs_cond_lives_increase_lv3(void) { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE && gMarioState->numLives <= (100 - LV3_LIVES_POS)); }
u8 chs_cond_lives_decrease_lv1(void) { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE && gMarioState->numLives >= LV1_LIVES_NEG); }
u8 chs_cond_lives_decrease_lv2(void) { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE && gMarioState->numLives >= LV2_LIVES_NEG); }
u8 chs_cond_lives_decrease_lv3(void) { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE && gMarioState->numLives >= LV3_LIVES_NEG); }
u8 chs_cond_instant_game_over(void)  { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE); }
u8 chs_cond_life_gambler(void)       { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE); }
u8 chs_cond_lifetime_sale(void)      { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE); }

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

s32 chs_life_gambler_get_lives_lost(void) {
    if (!chaos_check_if_patch_active(CHAOS_PATCH_LIFE_GAMBLER)) {
        return 1;
    }

    s32 livesLost = 1;
    s32 gamblerCount = chaos_count_active_instances(CHAOS_PATCH_LIFE_GAMBLER);
    for (s32 i = 0; i < gamblerCount; i++) {
        livesLost *= 2;
    }

    if (livesLost > __INT8_MAX__) {
        livesLost = __INT8_MAX__;
    }

    return livesLost;
}
