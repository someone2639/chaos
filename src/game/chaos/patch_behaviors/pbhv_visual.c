#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "course_table.h"
#include "sounds.h"
#include "audio/external.h"
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
