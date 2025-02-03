#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "course_table.h"
#include "sounds.h"
#include "audio/external.h"
#include "engine/behavior_script.h"
#include "game/debug.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/save_file.h"

u8 chs_cond_button_broken_a(void) {
    if (chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_B)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_Z)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_SWAPPED_ZR_AB)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_QUICKTIME)) {
        return FALSE;
    }

    return TRUE;
}

u8 chs_cond_button_broken_b(void) {
    if (chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_A)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_Z)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_SWAPPED_ZR_AB)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_QUICKTIME)) {
        return FALSE;
    }

    return TRUE;
}

u8 chs_cond_button_broken_z(void) {
    if (chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_A)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_B)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_SWAPPED_ZR_AB)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_QUICKTIME)) {
        return FALSE;
    }

    return TRUE;
}

u8 chs_cond_button_broken_c(void) {
    if (chaos_check_if_patch_active(CHAOS_PATCH_QUICKTIME)) {
        return FALSE;
    }
    
    return TRUE;
}

u8 chs_cond_swapped_zr_ab(void) {
    if (chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_A)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_B)) {
        return FALSE;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_Z)) {
        return FALSE;
    }

    return TRUE;
}

void chs_act_input_lag(void) {
    bzero(chaosControllerLag, sizeof(chaosControllerLag));
}
