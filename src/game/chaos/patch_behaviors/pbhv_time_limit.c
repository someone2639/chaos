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
#include "audio/external.h"
#include "game/debug.h"
#include "game/object_list_processor.h"

#define CHS_TIME_LIMIT                  (3 * 60 * 30)
#define CHS_TIME_LIMIT_OFFSET_MAX       (90 * 30)
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
    if (gCurrCourseNum == COURSE_NONE || gMarioState->action == ACT_JUMBO_STAR_CUTSCENE || gMarioState->action == ACT_CREDITS_CUTSCENE) {
        this->frameTimer = -1;
    } else {
        if(gMarioState->action == ACT_STAR_DANCE_NO_EXIT) {
            this->frameTimer = sTimeLimitOffset;
        }
        
        s32 timeLeft = (CHS_TIME_LIMIT - this->frameTimer);

        //Play ringing sfx at level start, 1 minute, and 30 second marks
        if(timeLeft == (30 * 30) || timeLeft == (60 * 30) || this->frameTimer == sTimeLimitOffset) {
            if(!(gTimeStopState & (TIME_STOP_ENABLED | TIME_STOP_DIALOG | TIME_STOP_ALL_OBJECTS)) && gMarioState->action != ACT_STAR_DANCE_NO_EXIT) {
                play_sound(SOUND_MENU_TIMER_RING, gGlobalSoundSource);
            }
        }

        if(timeLeft == 0) {
            if(chs_is_miracle_active()) {
                this->frameTimer = sTimeLimitOffset;
                gMarioState->health = 0x100;
                gMarioState->hurtCounter = 0;
                gMarioState->healCounter = chs_calculate_max_heal_counter();
                chs_decrement_miracle();
            } else {
                level_trigger_warp(gMarioState, WARP_OP_TIME_UP);
            }
        }
    }
}

u8 chs_cond_lower_time_limit(void) {
    return (chaos_check_if_patch_active(CHAOS_PATCH_TIME_LIMIT) && sTimeLimitOffset < CHS_TIME_LIMIT_OFFSET_MAX);
}

void chs_act_lower_time_limit(void) {
    sTimeLimitOffset += (30 * 30);

    // Identify longest remaining duration of the active lower time limit patches (and the original time limit patch).
    // If the game save wasn't reloaded, it should always become the max duration value.
    // If the game save WAS reloaded, everything below ideally wouldn't even execute, but at least this specific implementation won't break anything.
    s32 longestRemainingDuration = 1;
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        if (gChaosActiveEntries[i].id == CHAOS_PATCH_LOWER_TIME_LIMIT) {
            longestRemainingDuration = MAX(longestRemainingDuration, gChaosActiveEntries[i].remainingDuration);
        }
    }

    struct ChaosActiveEntry *chaosTimer;
    chaos_find_first_active_patch(CHAOS_PATCH_TIME_LIMIT, &chaosTimer);
    if (!chaosTimer) {
        assert(FALSE, "chs_act_lower_time_limit: Could not find parent time limit patch!");

        for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
            if (gChaosActiveEntries[i].id != CHAOS_PATCH_LOWER_TIME_LIMIT) {
                continue;
            }

            chaos_remove_expired_entry(i, NULL);
            i--;
        }
        return;
    }

    // Refresh the remaining duration of original time limit and other lower time limit patches.
    // This is mostly done so that the amount of time limit decrements
    // can be saved to the file and all of the patches will expire at once.
    longestRemainingDuration = MAX(longestRemainingDuration, chaosTimer->remainingDuration);
    chaosTimer->remainingDuration = longestRemainingDuration;

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        if (gChaosActiveEntries[i].id == CHAOS_PATCH_LOWER_TIME_LIMIT) {
            gChaosActiveEntries[i].remainingDuration = longestRemainingDuration;
        }
    }
}

void chs_deact_lower_time_limit(void) {
    sTimeLimitOffset = 0;
}
