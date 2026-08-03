#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "audio/heap.h"
#include "game/chaos/chaos.h"
#include "game/emutest.h"
#include "game/ingame_menu.h"
#include "game/level_update.h"
#include "game/area.h"
#include "game/main.h"
#include "game/object_helpers.h"
#include "game/save_file.h"
#include "course_table.h"
#include "behavior_data.h"
#include "engine/surface_collision.h"
#include "game/colors.h"
#include "engine/math_util.h"

u8 chs_cond_miracle_normal(void) {
    struct ChaosActiveEntry *match;

    if (gChaosGameMode == CHAOS_GAMEMODE_HARDCORE) {
        return FALSE;
    }

    if (chaos_find_first_active_patch(CHAOS_PATCH_MIRACLE_NORMAL, &match) >= 0) {
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

u8 chs_is_safety_net_active(void) {
    return (chaos_check_if_patch_active(CHAOS_PATCH_SAFETY_NET) && (gMarioState->health / 0x100) == (gMarioState->maxHealth / 0x100));
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

void chs_update_luigi(void) {
    gMarioState->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_LUIGI];
}

void chs_deact_luigi(void) {
    gMarioState->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_MARIO];
}


u8 chs_cond_marth_grab(void) {
    return save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) <= (BITS_STAR_REQUIREMENT - (s32) (((f32) gChaosPatches[CHAOS_PATCH_MARTH_GRAB].duration * 1.334f) + 2.5f));
}

void chs_update_noclip(void) {
    if (!count_objects_with_behavior(bhvNoclipPlane)) {
        spawn_object_abs_with_rot(gMarioState->marioObj, 0, MODEL_NONE, bhvNoclipPlane,
                            gMarioState->pos[0], FLOOR_LOWER_LIMIT + 50, gMarioState->pos[2], 0, 0, 0);
    }
}

u8 chs_cond_randomize_warps(void) {
    return (gChaosDifficulty != CHAOS_DIFFICULTY_IMPOSSIBLE);
}

void chs_act_number_blindness(void) {
    gChsNumberBlindness = TRUE;
}

void chs_deact_number_blindness(void) {
    gChsNumberBlindness = FALSE;
}

Lights1 gRainbowOverallLights = gdSPDefLights1(
    0x00, 0x00, 0x7f,
    0x00, 0x00, 0xff, 0x28, 0x28, 0x28
);

Lights1 gRainbowHatLights = gdSPDefLights1(
    0x7f, 0x00, 0x00,
    0xff, 0x00, 0x00, 0x28, 0x28, 0x28
);

#define CHANGE_LIGHT_COL(light, color)         \
    light.l[0].l.col[0]    = color.c.r;       \
    light.l[0].l.col[1]    = color.c.g;       \
    light.l[0].l.col[2]    = color.c.b;       \
    light.l[0].l.colc[0]   = color.c.r;       \
    light.l[0].l.colc[1]   = color.c.g;       \
    light.l[0].l.colc[2]   = color.c.b;       \
    light.a.l.col[0]       = (color.c.r / 2); \
    light.a.l.col[1]       = (color.c.g / 2); \
    light.a.l.col[2]       = (color.c.b / 2); \
    light.a.l.colc[0]      = (color.c.r / 2); \
    light.a.l.colc[1]      = (color.c.g / 2); \
    light.a.l.colc[2]      = (color.c.b / 2)

void chs_update_mario_rainbow(void) {
    RGBA overallRGB;
    RGBA hatRGB;
    HSV overallHSV = {.h = 0, .s = 1.0f, .v = 1.0f, .a = 1.0f};
    HSV hatHSV = {.h = 0, .s = 1.0f, .v = 1.0f, .a = 1.0f};

    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_MARIO_RAINBOW, &this);

    overallHSV.h = (this->frameTimer * 2) % 360;
    hatHSV.h = ((this->frameTimer * 2) + 180) % 360;
    overallRGB = HSV_to_RGB(overallHSV);
    hatRGB = HSV_to_RGB(hatHSV);

    CHANGE_LIGHT_COL(gRainbowOverallLights, overallRGB);
    CHANGE_LIGHT_COL(gRainbowHatLights, hatRGB);
}