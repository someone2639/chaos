#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "course_table.h"
#include "sm64.h"
#include "game/area.h"
#include "game/level_update.h"
#include "game/mario.h"
#include "game/print.h"

#include "game/chaos/chaos.h"

#define SLEEP_TIME_MIN   1800
#define SLEEP_TIME_RAND  10800

#define SHOCK_TIME_MIN   900
#define SHOCK_TIME_RAND  3600

#define BURN_TIME_MIN   900
#define BURN_TIME_RAND  3600

s16 sRandomSleepTimer = -1;
s16 sRandomShockTimer = -1;
s16 sRandomBurnTimer = -1;

/*
    Sleeping
*/
void chs_act_random_sleep(void) {
    sRandomSleepTimer = RAND(SLEEP_TIME_RAND) + SLEEP_TIME_MIN;
}

void chs_update_random_sleep(void) {
    if(sRandomSleepTimer == 0) {
        s32 actGroup = (gMarioState->action & ACT_GROUP_MASK);
        if(actGroup == ACT_GROUP_MOVING || actGroup == ACT_GROUP_STATIONARY) {
            set_mario_action(gMarioState, ACT_START_SLEEPING, 0);
            sRandomSleepTimer = -1;
        }
    } else if (sRandomSleepTimer == -1) {
        if(gMarioState->action != ACT_START_SLEEPING) {
            sRandomSleepTimer = RAND(SLEEP_TIME_RAND) + SLEEP_TIME_MIN;
        }
    } else {
        sRandomSleepTimer--;
    }
}

/*
    Shocking
*/

void chs_act_random_shock(void) {
    sRandomShockTimer = RAND(SHOCK_TIME_RAND) + SHOCK_TIME_MIN;
}

void chs_update_random_shock(void) {
    if(sRandomShockTimer == 0) {
        sRandomShockTimer = -1;
    } else if (sRandomShockTimer == -1) {
        s32 actGroup = (gMarioState->action & ACT_GROUP_MASK);
        if(!(actGroup == ACT_GROUP_CUTSCENE || actGroup == ACT_GROUP_AUTOMATIC)) {
            hurt_and_set_mario_action(gMarioState, ACT_SHOCKED, 0, 4);
            sRandomShockTimer = RAND(SHOCK_TIME_RAND) + SHOCK_TIME_MIN;
        }
    } else {
        sRandomShockTimer--;
    }
}

u8 chs_cond_random_shock(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_ONE_HIT_WONDER));
}

/*
    Burning
*/

void chs_act_random_burn(void) {
    sRandomBurnTimer = RAND(BURN_TIME_RAND) + BURN_TIME_MIN;
}

void chs_update_random_burn(void) {
    if (sRandomBurnTimer == 0) {
        sRandomBurnTimer = -1;
    } else if (sRandomBurnTimer == -1) {
        s32 actGroup = (gMarioState->action & ACT_GROUP_MASK);
        if(!(actGroup == ACT_GROUP_CUTSCENE || actGroup == ACT_GROUP_SUBMERGED || actGroup == ACT_GROUP_AUTOMATIC)) {
            gMarioState->marioObj->oMarioBurnTimer = 0;
            u32 burningAction = ACT_BURNING_JUMP;

            play_mario_sound(gMarioState, SOUND_MARIO_ON_FIRE, 0);
            if (!chs_check_temporary_invincibility()) {
                if (chaos_check_if_patch_active(CHAOS_PATCH_SONIC_SIMULATOR) && gCurrCourseNum != COURSE_NONE) {
                    set_hurt_counter(gMarioState, (gMarioState->flags & MARIO_CAP_ON_HEAD) ? 12 : 18);
                }
            }

            if ((gMarioState->action & ACT_FLAG_AIR) && gMarioState->vel[1] <= 0.0f) {
                burningAction = ACT_BURNING_FALL;
            }

            set_mario_action(gMarioState, burningAction, 0);

            sRandomBurnTimer = RAND(BURN_TIME_RAND) + BURN_TIME_MIN;
        }
    } else {
        sRandomBurnTimer--;
    }
}

u8 chs_cond_random_burn(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_ONE_HIT_WONDER));
}

/*
    Blinding
*/

#define BLIND_TIME_MAX      18000
#define BLIND_TIME_END      18450

void chs_act_random_blind(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_RANDOM_BLIND, &this);
    this->frameTimer = RAND(BLIND_TIME_MAX); //Get a random offset to start the timer at
}

void chs_update_random_blind(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_RANDOM_BLIND, &this);
    if(this->frameTimer > BLIND_TIME_END) {
        this->frameTimer = RAND(BLIND_TIME_MAX); //Get a random offset to restart the timer at
    }
}
