#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "game/chaos/chaos.h"
#include "game/level_update.h"

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
    if(gMarioState->health == 0xFF) {
        gMarioState->health = 0x880;
        gMarioState->hurtCounter = 0;
        chaos_decrement_patch_usage(CHAOS_PATCH_MIRACLE);
    }
}