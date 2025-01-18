#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/level_update.h"
#include "game/chaos/chaos.h"
#include "game/game_init.h"

s8 sChsLevitating = FALSE;

void chs_update_l_to_levitate(void) {
    if(gPlayer1Controller->buttonDown & L_TRIG) {
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