#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/level_update.h"
#include "behavior_data.h"
#include "game/chaos/chaos.h"
#include "game/print.h"
#include "game/fasttext.h"
#include "game/area.h"
#include "course_table.h"

#define CHS_TIME_LIMIT          5400
s32 sTimeLimitOffset = 0;

void chs_level_init_time_limit(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_TIME_LIMIT, &this);
    this->frameTimer = sTimeLimitOffset;
}

void chs_update_time_limit(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_TIME_LIMIT, &this);
    //Should not be active in the castle
    if (gCurrCourseNum == COURSE_NONE) {
        this->frameTimer = -1;
    } else {
        if(gMarioState->action == ACT_STAR_DANCE_NO_EXIT) {
            this->frameTimer = sTimeLimitOffset;
        }
        s32 timeLeft = (CHS_TIME_LIMIT - this->frameTimer);
        if(timeLeft == 0) {
            gMarioState->health = 0xFF;
        }
    }
}

void chs_deact_time_limit(void) {
    sTimeLimitOffset = 0;
}

u8 chs_cond_lower_time_limit(void) {
    return (chaos_check_if_patch_active(CHAOS_PATCH_TIME_LIMIT) && sTimeLimitOffset < 60);
}

void chs_act_lower_time_limit(void) {
    struct ChaosActiveEntry *chaosTimer;
    chaos_find_first_active_patch(CHAOS_PATCH_TIME_LIMIT, &chaosTimer);
    chaosTimer->remainingDuration = 15;
    sTimeLimitOffset += 450;
}

void chs_deact_lower_time_limit(void) {
    if(sTimeLimitOffset > 0) {
        sTimeLimitOffset -= 450;
    }
}
