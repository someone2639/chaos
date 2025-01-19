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

s8 sChsLevitating = FALSE;

void chs_update_l_to_levitate(void) {
    s32 actGroup = (gMarioState->action & ACT_GROUP_MASK);

    if(gPlayer1Controller->buttonDown & L_TRIG && !(actGroup == ACT_GROUP_CUTSCENE)) {
        sChsLevitating = TRUE;
        gMarioState->vel[0] = 0.0f;
        gMarioState->vel[1] = 25.0f;
        gMarioState->vel[2] = 0.0f;
        gMarioState->forwardVel = 0;
        set_mario_action(gMarioState, ACT_JUMP, 0);
    } else {
        if(sChsLevitating) {
            chaos_decrement_patch_usage(CHAOS_PATCH_L_TO_LEVITATE);
            sChsLevitating = FALSE;
        }
    }
}

u8 chs_cond_l_to_levitate(void) {
    struct ChaosActiveEntry *match;
    chaos_find_first_active_patch(CHAOS_PATCH_L_TO_LEVITATE, &match);
    if(match) {
        return (match->remainingDuration < 4);
    } else {
        return TRUE;
    }
}

/*
    Debug free move
*/

u8 sChsDebug = FALSE;

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
    if(gPlayer1Controller->buttonDown & U_JPAD) {
        set_mario_action(gMarioState, ACT_DEBUG_FREE_MOVE, 0);
        sChsDebug = TRUE;
    }

    if(sChsDebug && gMarioState->action != ACT_DEBUG_FREE_MOVE) {
        chaos_decrement_patch_usage(CHAOS_PATCH_DEBUG_FREE_MOVE);
        sChsDebug = FALSE;
    }
}