#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/camera.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/level_update.h"
#include "behavior_data.h"
#include "game/chaos/chaos.h"

u8 chs_cam_invert_x_check(void) {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_TOP_DOWN_CAMERA));
}

u8 chs_topdown_check(void) {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_INVERTED_CAMERA_X)
         && !chaos_check_if_patch_active(CHAOS_PATCH_FORCED_MARIO_CAM)
         && !chaos_check_if_patch_active(CHAOS_PATCH_DECREASED_FOV)
         && !chaos_check_if_patch_active(CHAOS_PATCH_INCREASED_FOV)
         && !chaos_check_if_patch_active(CHAOS_PATCH_45_DEGREE_CAM)
    );
}

u8 chs_cond_forced_mario_cam(void) {
    return !(chaos_check_if_patch_active(CHAOS_PATCH_TOP_DOWN_CAMERA) || chaos_check_if_patch_active(CHAOS_PATCH_CAMERA_LAG) || chaos_check_if_patch_active(CHAOS_PATCH_45_DEGREE_CAM));
}

u8 chs_cond_45_degree_camera(void) {
    return !(chaos_check_if_patch_active(CHAOS_PATCH_FORCED_MARIO_CAM) || chaos_check_if_patch_active(CHAOS_PATCH_TOP_DOWN_CAMERA));
}

void chs_act_45_degree_camera(void) {
    gChsForced8DirCam |= FORCED_8DIR_FLAGS_45DEG_CAM;
}

void chs_deact_45_degree_camera(void) {
    gChsForced8DirCam &= ~FORCED_8DIR_FLAGS_45DEG_CAM;
}
