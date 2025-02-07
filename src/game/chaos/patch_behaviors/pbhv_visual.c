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

u8 chs_cond_no_zbuffer(void)       { return !chaos_check_if_patch_active(CHAOS_PATCH_INVERTED_Z_BUFFER); }
u8 chs_cond_inverted_zbuffer(void) { return !chaos_check_if_patch_active(CHAOS_PATCH_NO_Z_BUFFER      ); }

u8 chs_cond_decreased_fov(void) {
    if (chaos_check_if_patch_active(CHAOS_PATCH_INCREASED_FOV)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_TOP_DOWN_CAMERA)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_ORTHO)) {
        return FALSE;
    }
    return TRUE;
}
u8 chs_cond_increased_fov(void) {
    if (chaos_check_if_patch_active(CHAOS_PATCH_DECREASED_FOV)) {
        return FALSE;
    }
    if ((gEmulator & EMU_CONSOLE) && chaos_check_if_patch_active(CHAOS_PATCH_TOP_DOWN_CAMERA)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_ORTHO)) {
        return FALSE;
    }
    return TRUE;
}

u8 chs_cond_low_resolution(void) { return (!(gEmulator & (EMU_CONSOLE | EMU_ARES)) && gFBEEnabled); }

u8 chs_cond_ortho(void) {
    if (chaos_check_if_patch_active(CHAOS_PATCH_INCREASED_FOV)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_DECREASED_FOV)) {
        return FALSE;
    }
    // if (chaos_check_if_patch_active(CHAOS_PATCH_TOP_DOWN_CAMERA)) {
    //     return FALSE;
    // }


    return TRUE;
}
