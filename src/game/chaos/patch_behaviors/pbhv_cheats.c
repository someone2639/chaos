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
        return (match->remainingDuration < 4);
    } else {
        return TRUE;
    }
}

void chs_update_l_to_levitate(void) {
    s32 group = (gMarioState->action & ACT_GROUP_MASK);
    if (gPlayer1Controller->buttonPressed & L_TRIG 
            && !(gMarioState->action & ACT_FLAG_INTANGIBLE)
            && group != ACT_GROUP_CUTSCENE
            && group != ACT_GROUP_SUBMERGED) {
        if (gMarioState->action != ACT_JUMP) {
            set_mario_action(gMarioState, ACT_JUMP, 1);
            chaos_decrement_patch_usage(CHAOS_PATCH_L_TO_LEVITATE);
        } else if (gMarioState->actionArg != 1) {
            gMarioState->actionArg = 1;
            chaos_decrement_patch_usage(CHAOS_PATCH_L_TO_LEVITATE);
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

    set_mario_action(gMarioState, ACT_DEBUG_FREE_MOVE, 0);
    chaos_decrement_patch_usage(CHAOS_PATCH_DEBUG_FREE_MOVE);
}

/*
    Level Reset
*/

u8 chs_cond_level_reset(void) {
    struct ChaosActiveEntry *match;
    chaos_find_first_active_patch(CHAOS_PATCH_LEVEL_RESET, &match);
    if(match) {
        return (match->remainingDuration < 9);
    } else {
        return TRUE;
    }
}