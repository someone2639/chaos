#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "audio/heap.h"
#include "game/chaos/chaos.h"
#include "game/emutest.h"
#include "game/level_update.h"
#include "game/area.h"
#include "game/main.h"
#include "game/object_helpers.h"
#include "game/save_file.h"
#include "course_table.h"
#include "behavior_data.h"
#include "engine/surface_collision.h"

u8 chs_cond_one_hit_wonder(void) {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_RANDOM_SHOCK)
         && !chaos_check_if_patch_active(CHAOS_PATCH_RANDOM_BURN)
         && !chaos_check_if_patch_active(CHAOS_PATCH_COSMIC_RAYS)
    );
}

u8 chs_cond_miracle_normal(void) {
    struct ChaosActiveEntry *match;

    if (gChaosGameMode == CHAOS_GAMEMODE_HARDCORE) {
        return FALSE;
    }

    if(chaos_find_first_active_patch(CHAOS_PATCH_MIRACLE_NORMAL, &match)) {
        return (match->remainingDuration + gChaosPatches[match->id].duration <= 4);
    } else {
        return TRUE;
    }
}

u8 chs_cond_miracle_hardcore(void) {
    return (gChaosGameMode == CHAOS_GAMEMODE_HARDCORE);
}

void chs_update_miracle(void) {
    if (gMarioState->health <= 0xFF) {
        gMarioState->health = 0x100;
        gMarioState->hurtCounter = 0;
        gMarioState->healCounter = chs_calculate_max_heal_counter();
        chs_decrement_miracle();
    }
}

u8 chs_is_miracle_active(void) {
    return (chaos_check_if_patch_active(CHAOS_PATCH_MIRACLE_NORMAL) || chaos_check_if_patch_active(CHAOS_PATCH_MIRACLE_HARDCORE));
}

void chs_decrement_miracle(void) {
    if (chaos_check_if_patch_active(CHAOS_PATCH_MIRACLE_HARDCORE)) {
        chaos_decrement_patch_usage(CHAOS_PATCH_MIRACLE_HARDCORE);
    } else {
        chaos_decrement_patch_usage(CHAOS_PATCH_MIRACLE_NORMAL);
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

u8 chs_cond_20_fps(void) { return (!chaos_check_if_patch_active(CHAOS_PATCH_45_FPS) && !chaos_check_if_patch_active(CHAOS_PATCH_60_FPS)); }
u8 chs_cond_45_fps(void) { return (!chaos_check_if_patch_active(CHAOS_PATCH_20_FPS) && !chaos_check_if_patch_active(CHAOS_PATCH_60_FPS)); }
u8 chs_cond_60_fps(void) { return (!chaos_check_if_patch_active(CHAOS_PATCH_20_FPS) && !chaos_check_if_patch_active(CHAOS_PATCH_45_FPS)); }

void chs_act_reverb(void) { init_reverb_us(1U << 31); }
void chs_deact_reverb(void) { init_reverb_us(1U << 31); }

u8 chs_cond_lethal_fall_damage(void) {
    return (!(chaos_check_if_patch_active(CHAOS_PATCH_NO_FALL_DAMAGE) || chaos_check_if_patch_active(CHAOS_PATCH_COSMIC_RAYS)));
}

u8 chs_cond_randomized_music(void) {
    return (!gConfig.disableBGMusic);
}

u8 chs_cond_marth_grab(void) {
    return save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) <= (BITS_STAR_REQUIREMENT - 10);
}

void chs_update_noclip(void) {
    if (!count_objects_with_behavior(bhvNoclipPlane)) {
        spawn_object_abs_with_rot(gMarioState->marioObj, 0, MODEL_NONE, bhvNoclipPlane,
                            gMarioState->pos[0], FLOOR_LOWER_LIMIT + 50, gMarioState->pos[2], 0, 0, 0);
    }
}

u8 chs_cond_randomize_warps(void) {
    return (gChaosGameMode != CHAOS_GAMEMODE_CLASSIC && gChaosDifficulty != CHAOS_DIFFICULTY_IMPOSSIBLE);
}
