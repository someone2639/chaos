#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/level_update.h"
#include "behavior_data.h"
#include "game/chaos/chaos.h"

u8 chs_cam_invert_x_check(void) {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_TOP_DOWN_CAMERA));
}

u8 chs_topdown_check(void) {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_INVERTED_CAMERA_X) && !chaos_check_if_patch_active(CHAOS_PATCH_FORCED_MARIO_CAM));
}

u8 chs_cond_forced_mario_cam(void) {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_TOP_DOWN_CAMERA));
}
