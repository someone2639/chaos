#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "game/chaos/chaos.h"
#include "game/level_update.h"
#include "engine/math_util.h"
#include "game/game_init.h"
#include "game/area.h"
#include "course_table.h"

/*
    Push Backwards
*/

u8 chs_cond_push_back(void) {
    return (chaos_count_active_instances(CHAOS_PATCH_PUSH_BACK) < 3);
}

void chs_update_push_back(void) {
    s32 group = (gMarioState->action & ACT_GROUP_MASK);
    if ((gMarioState->action & ACT_FLAG_INTANGIBLE) || group == ACT_GROUP_CUTSCENE) {
        return;
    }
    
    s16 pushAngle = gMarioState->faceAngle[1] + 0x8000;
    f32 speed = 8.0f;

    gMarioState->pos[0] += speed * sins(pushAngle);
    gMarioState->pos[2] += speed * coss(pushAngle);
}

/*
    Speed Limit
*/

u8 chs_cond_speed_limit(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_GREEN_DEMON) && !chaos_check_if_patch_active(CHAOS_PATCH_WALKIES) && !chaos_check_if_patch_active(CHAOS_PATCH_SPEED_TAX));
}

void chs_update_speed_limit(void) {
    if(gMarioState->forwardVel > 26.0f) {
        gMarioState->forwardVel = 26.0f;
    }
}

/*
    Walkies
*/

u8 chs_cond_walkies(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_GREEN_DEMON) && !chaos_check_if_patch_active(CHAOS_PATCH_SPEED_LIMIT) && !chaos_check_if_patch_active(CHAOS_PATCH_SPEED_TAX) && !chaos_check_if_patch_active(CHAOS_PATCH_HEALTH_DRAIN));
}

void chs_update_walkies(void) {
    if(gMarioState->forwardVel > 10.0f) {
        gMarioState->forwardVel = 10.0f;
    }
}

/*
    Speed Tax
*/

u8 chs_cond_speed_tax(void) {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_SPEED_LIMIT) && !chaos_check_if_patch_active(CHAOS_PATCH_WALKIES));
}

void chs_update_speed_tax(void) {
    if(gCurrCourseNum == COURSE_NONE) {
        return;
    }

    if(gMarioState->forwardVel > 32.0f) {
        if(!(gGlobalTimer % 30)) {
            if(gMarioState->numCoins > 0) {
                gMarioState->numCoins--;
                gHudDisplay.coins = gMarioState->numCoins;
            } else {
                gMarioState->hurtCounter++;
            }
        }
    }
}
