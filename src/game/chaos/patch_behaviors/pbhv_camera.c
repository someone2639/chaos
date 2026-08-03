#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/camera.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/level_update.h"
#include "game/main.h"
#include "behavior_data.h"
#include "game/chaos/chaos.h"

u8 chs_cond_forced_mario_cam(void) {
    return !gConfig.disableHarshVisuals;
}

void chs_act_45_degree_camera(void) {
    gChsForced8DirCam |= FORCED_8DIR_FLAGS_45DEG_CAM;
}

void chs_deact_45_degree_camera(void) {
    gChsForced8DirCam &= ~FORCED_8DIR_FLAGS_45DEG_CAM;
}

void chs_act_smooth_camera(void) {
    gChsForced8DirCam |= FORCED_8DIR_FLAGS_SMOOTH_CAM;
}

void chs_deact_smooth_camera(void) {
    gChsForced8DirCam &= ~FORCED_8DIR_FLAGS_SMOOTH_CAM;
}
