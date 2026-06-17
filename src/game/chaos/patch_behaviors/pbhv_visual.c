#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "course_table.h"
#include "sounds.h"
#include "audio/external.h"
#include "engine/behavior_script.h"
#include "engine/math_util.h"
#include "game/area.h"
#include "game/debug.h"
#include "game/emutest.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/main.h"

u8 chs_cond_no_zbuffer(void)       { return !gConfig.disableHarshVisuals; }

u8 chs_cond_increased_fov(void) { return (!(gEmulator & EMU_CONSOLE) || !chaos_check_if_patch_active(CHAOS_PATCH_TOP_DOWN_CAMERA)); }

u8 chs_cond_low_resolution(void) { return (!(gEmulator & (EMU_CONSOLE | EMU_ARES)) && gFBEEnabled && (!gConfig.disableHarshVisuals)); }

u8 chs_cond_no_skybox(void) {
    return !gConfig.disableHarshVisuals;
}

u8 chs_cond_dizzy_objects(void) {
    return !gConfig.disableHarshVisuals;
}

u8 chs_cond_camera_lag(void) {
    return !gConfig.disableHarshVisuals;
}

u8 chs_cond_darkness(void) {
    return !(gEmulator & (EMU_CONSOLE | EMU_ARES)); // Probably laggy
}

void chs_act_sideways_camera(void) {
    gChsSidewaysOrientation = random_u16() % 2;
}

#define CAMERA_ROCK_MAX_PITCH 0x1C00
#define CAMERA_ROCK_MIN_PITCH 0x0200
#define CAMERA_ROCK_MAX_ROCK_TIME 300
#define CAMERA_ROCK_MIN_ROCK_TIME 90
#define CAMERA_ROCK_FRAME_DIFFER_MAX 30

u32 sChsRockTimeLast = (CAMERA_ROCK_MAX_ROCK_TIME + CAMERA_ROCK_MIN_ROCK_TIME) / 2;
u32 sChsRockTimeLeft = 0;
s16 sChsRockStart = 0;
s16 sChsRockEnd = 0;

u8 chs_cond_rocking_camera(void) {
    return !gConfig.disableHarshVisuals;
}

void chs_act_rocking_camera(void) {
    sChsRockTimeLast = (CAMERA_ROCK_MAX_ROCK_TIME + CAMERA_ROCK_MIN_ROCK_TIME) / 2;
    sChsRockTimeLeft = 0;
    sChsRockStart = 0;
    sChsRockEnd = 0;
    gChsCurrentRock = 0;
}

void chs_update_rocking_camera(void) {
    if (gConfig.disableHarshVisuals) {
        chs_act_rocking_camera();
        return;
    }

    if (sChsRockTimeLeft == 0) {
        sChsRockStart = sChsRockEnd;
        sChsRockEnd = (random_float() * (CAMERA_ROCK_MAX_PITCH - CAMERA_ROCK_MIN_PITCH)) + CAMERA_ROCK_MIN_PITCH;
        if (sChsRockStart >= 0) {
            sChsRockEnd *= -1;
        }

        sChsRockTimeLeft = sChsRockTimeLast + (CAMERA_ROCK_FRAME_DIFFER_MAX * (random_float() - 0.5f) * 2);
        if (sChsRockTimeLeft > CAMERA_ROCK_MAX_ROCK_TIME) {
            sChsRockTimeLeft = CAMERA_ROCK_MAX_ROCK_TIME;
        } else if (sChsRockTimeLeft < CAMERA_ROCK_MIN_ROCK_TIME) {
            sChsRockTimeLeft = CAMERA_ROCK_MIN_ROCK_TIME;
        }

        sChsRockTimeLast = sChsRockTimeLeft;
    }

    f32 mult = (coss(0x8000 * ((f32) sChsRockTimeLeft / (f32) sChsRockTimeLast)) + 1.0f) / 2.0f;
    gChsCurrentRock = (sChsRockStart * (1.0f - mult)) + (sChsRockEnd * mult);

    sChsRockTimeLeft--;
}
