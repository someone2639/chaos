#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "audio/heap.h"
#include "game/chaos/chaos.h"
#include "game/level_update.h"
#include "game/area.h"
#include "game/main.h"
#include "game/object_helpers.h"
#include "course_table.h"
#include "behavior_data.h"
#include "engine/surface_collision.h"
#include "game/emutest.h"

u8 chs_cond_one_hit_wonder(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_RANDOM_SHOCK) && !chaos_check_if_patch_active(CHAOS_PATCH_RANDOM_BURN));
}

u8 chs_cond_miracle(void) {
    struct ChaosActiveEntry *match;
    chaos_find_first_active_patch(CHAOS_PATCH_MIRACLE, &match);
    if(match) {
        return (match->remainingDuration < 3);
    } else {
        return TRUE;
    }
}

void chs_update_miracle(void) {
    if (gMarioState->health <= 0xFF) {
        gMarioState->health = 0x100;
        gMarioState->hurtCounter = 0;
        gMarioState->healCounter = chs_calculate_max_heal_counter();
        chaos_decrement_patch_usage(CHAOS_PATCH_MIRACLE);
    }
}

u8 chs_cond_invisible(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_LUIGI));
}

u8 chs_cond_luigi(void) {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_MARIO_INVISIBLE));
}

void chs_update_luigi(void) {
    gMarioState->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_LUIGI];
}

void chs_deact_luigi(void) {
    gMarioState->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_MARIO];
}

u8 chs_cond_45_fps(void) {
    if (chaos_check_if_patch_active(CHAOS_PATCH_20_FPS)) {
        return FALSE;
    }
    if ((gEmulator & (EMU_ARES | EMU_CONSOLE))) {
        return FALSE;
    }
    return TRUE;
}

u8 chs_cond_20_fps(void) { return (!chaos_check_if_patch_active(CHAOS_PATCH_45_FPS)); }

void chs_act_reverb(void) { init_reverb_us(1U << 31); }
void chs_deact_reverb(void) { init_reverb_us(1U << 31); }

u8 chs_cond_lethal_fall_damage() {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_NO_FALL_DAMAGE));
}

u8 chs_cond_randomized_music(void) {
    return (!gConfig.disableBGMusic);
}

void chs_update_noclip(void) {
    if (!count_objects_with_behavior(bhvNoclipPlane)) {
        spawn_object_abs_with_rot(gMarioState->marioObj, 0, MODEL_NONE, bhvNoclipPlane,
                            gMarioState->pos[0], FLOOR_LOWER_LIMIT + 50, gMarioState->pos[2], 0, 0, 0);
    }
}

u8 chs_cond_randomize_warps(void) {
    return gChaosLivesEnabled;
}
