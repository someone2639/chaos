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
#include "game/print.h"
#include "audio/external.h"

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
    Walkies
*/

u8 chs_cond_walkies(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_GREEN_DEMON) && !chaos_check_if_patch_active(CHAOS_PATCH_SPEED_TAX) && !chaos_check_if_patch_active(CHAOS_PATCH_HEALTH_DRAIN) && !chaos_check_if_patch_active(CHAOS_PATCH_COSMIC_CLONES));
}

/*
    Speed Tax
*/

u8 sSpeedTaxCounter = 0;

void chs_act_speed_tax(void) {
    sSpeedTaxCounter = 0;
}

u8 chs_cond_speed_tax(void) {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_WALKIES));
}

void chs_update_speed_tax(void) {
    if(gCurrCourseNum == COURSE_NONE) {
        return;
    }

    if(gMarioState->forwardVel > 36.0f) {
        if(sSpeedTaxCounter++ > 30) {
            if(gMarioState->numCoins > 0) {
                gMarioState->numCoins--;
                gHudDisplay.coins = gMarioState->numCoins;
            } else {
                gMarioState->hurtCounter++;
            }

            play_sound(SOUND_GENERAL_COIN, gGlobalSoundSource);
            sSpeedTaxCounter = 0;
        }

        print_text_centered(SCREEN_CENTER_X, 20, "SLOW DOWN");
    }
}
