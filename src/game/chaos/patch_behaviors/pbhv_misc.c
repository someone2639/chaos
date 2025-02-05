#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "audio/heap.h"
#include "game/chaos/chaos.h"
#include "game/level_update.h"
#include "game/area.h"

u8 chs_cond_one_hit_wonder(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_RANDOM_SHOCK) && !chaos_check_if_patch_active(CHAOS_PATCH_RANDOM_BURN));
}

u8 chs_cond_miracle(void) {
    struct ChaosActiveEntry *match;
    chaos_find_first_active_patch(CHAOS_PATCH_MIRACLE, &match);
    if(match) {
        return (match->remainingDuration < 3);
    } else {
        return TRUE;
    }
}

void chs_update_miracle(void) {
    if (gMarioState->health <= 0xFF) {
        gMarioState->health = 0x100;
        gMarioState->hurtCounter = 0;
        gMarioState->healCounter = chs_calculate_max_heal_counter();
        chaos_decrement_patch_usage(CHAOS_PATCH_MIRACLE);
    }
}

u8 chs_cond_invisible(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_LUIGI));
}

u8 chs_cond_luigi(void) {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_MARIO_INVISIBLE));
}

void chs_update_luigi(void) {
    gMarioState->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_LUIGI];
}

void chs_deact_luigi(void) {
    gMarioState->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_MARIO];
}

u8 chs_cond_45_fps(void) { return (!chaos_check_if_patch_active(CHAOS_PATCH_20_FPS)); }
u8 chs_cond_20_fps(void) { return (!chaos_check_if_patch_active(CHAOS_PATCH_45_FPS)); }

void chs_act_reverb(void) { init_reverb_us(1 << 31); }
void chs_deact_reverb(void) { init_reverb_us(1 << 31); }

u8 chs_cond_lethal_fall_damage() {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_NO_FALL_DAMAGE));
}