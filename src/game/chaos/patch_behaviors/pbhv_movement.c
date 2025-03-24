#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "course_table.h"
#include "sm64.h"
#include "sounds.h"
#include "audio/external.h"
#include "engine/behavior_script.h"
#include "game/area.h"
#include "game/debug.h"
#include "game/level_update.h"
#include "game/mario.h"

#define FRAMES_BETWEEN_TRIPS_MIN (30 * 45)
#define TRIP_PROBABILITY 0.005f

void chs_update_brawl_tripping(void) {
    const s32 action = gMarioState->action;
    const s32 goodflags = (ACT_FLAG_MOVING);
    const s32 badflags = (
        ACT_FLAG_STATIONARY |
        ACT_FLAG_AIR |
        ACT_FLAG_INTANGIBLE |
        ACT_FLAG_SWIMMING |
        ACT_FLAG_METAL_WATER |
        ACT_FLAG_RIDING_SHELL |
        ACT_FLAG_BUTT_OR_STOMACH_SLIDE |
        ACT_FLAG_DIVING |
        ACT_FLAG_ON_POLE |
        ACT_FLAG_HANGING |
        ACT_FLAG_IDLE |
        ACT_FLAG_SWIMMING_OR_FLYING |
        ACT_FLAG_WATER_OR_TEXT |
        ACT_FLAG_THROWING
    );

    if ((action & badflags) || !(action & goodflags)) {
        return;
    }

    if (random_float() > TRIP_PROBABILITY) {
        return;
    }

    struct ChaosActiveEntry *match;
    chaos_find_first_active_patch(CHAOS_PATCH_BRAWL_TRIPPING, &match);
    if (!match || match->frameTimer < FRAMES_BETWEEN_TRIPS_MIN) {
        return;
    }

    set_mario_action(gMarioState, ACT_HARD_FORWARD_GROUND_KB, 0);
    match->frameTimer = 0;
    if (gMarioState->marioObj) {
        play_sound(SOUND_MARIO_OOOF2, gMarioState->marioObj->header.gfx.cameraToObject);
    }
}

void chs_update_galaxy_twirl(void) {
    if((gMarioState->action & ACT_GROUP_MASK) == ACT_GROUP_AIRBORNE) {
        if(gMarioState->spinTimer <= 2) {
            gMarioState->spinTimer++;
        }
    } else {
        gMarioState->spinTimer = 0;
        gMarioState->usedSpin = FALSE;
    }
}

u8 chs_cond_no_fall_damage(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_LETHAL_FALL_DAMAGE));
}

u8 chs_cond_harder_longjumps(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_LONG_JUMP));
}

u8 chs_cond_losemove_longjump(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_HARDER_LONG_JUMPS));
}

u8 chs_cond_losemove_wall_kick(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_STICKY_WALL_JUMP));
}