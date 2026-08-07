#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/level_update.h"
#include "game/chaos/chaos.h"
#include "game/game_init.h"

/*
    L to Levitate
*/

u8 chs_cond_l_to_levitate(void) {
    struct ChaosActiveEntry *match;
    chaos_find_first_active_patch(CHAOS_PATCH_L_TO_LEVITATE, &match);
    if(match) {
        return (match->remainingDuration + chaos_calculate_patch_duration(&gChaosPatches[match->id]) <= 3);
    } else {
        return TRUE;
    }
}

u8 chs_can_mario_levitate(void) {
    s32 group = (gMarioState->action & ACT_GROUP_MASK);

    if(gMarioState->chaosStateFlags & CHAOS_STATE_CONTROLLING_TETRIS) {
        return FALSE;
    }

    if (!(gMarioState->action & ACT_FLAG_INTANGIBLE)
            && group != ACT_GROUP_CUTSCENE
            && group != ACT_GROUP_SUBMERGED) {
        return TRUE;
    }

    return FALSE;
}

void chs_update_l_to_levitate(void) {
    if ((gPlayer1Controller->buttonPressed & L_TRIG) && chs_can_mario_levitate() && !(gMarioState->action & ACT_FLAG_INVULNERABLE)) {
        if (!(gMarioState->chaosStateFlags & CHAOS_STATE_L_TO_LEVITATE)) {
            gMarioState->chaosStateFlags |= CHAOS_STATE_L_TO_LEVITATE;
            chaos_decrement_patch_usage(CHAOS_PATCH_L_TO_LEVITATE);

            if (!(gMarioState->action & ACT_FLAG_AIR)) {
                set_mario_action(gMarioState, ACT_JUMP, 0);
            }
        }
    }
}

/*
    Debug free move
*/

u8 chs_cond_debug_free_move(void) {
    struct ChaosActiveEntry *match;
    chaos_find_first_active_patch(CHAOS_PATCH_DEBUG_FREE_MOVE, &match);
    if(match) {
        return (match->remainingDuration < 2);
    } else {
        return TRUE;
    }
}

void chs_update_debug_free_move(void) {
    s32 group = (gMarioState->action & ACT_GROUP_MASK);

    if(gMarioState->action == ACT_DEBUG_FREE_MOVE || !(gPlayer1Controller->buttonPressed & U_JPAD)) {
        return;
    }
    
    if ((gMarioState->action & ACT_FLAG_INTANGIBLE) || group == ACT_GROUP_CUTSCENE) {
        return;
    }

    if(gMarioState->chaosStateFlags & CHAOS_STATE_CONTROLLING_TETRIS) {
        return;
    }

    set_mario_action(gMarioState, ACT_DEBUG_FREE_MOVE, 0);
    chaos_decrement_patch_usage(CHAOS_PATCH_DEBUG_FREE_MOVE);
}
