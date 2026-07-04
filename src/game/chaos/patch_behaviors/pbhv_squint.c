#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "sounds.h"
#include "audio/external.h"
#include "game/level_update.h"

#include "pbhv_viewport_mods.h"

void chs_squint_init(void) {
    chaos_viewport_width_target = SCREEN_WIDTH / SQUINT_FACTOR;
    chaos_viewport_height_target = SCREEN_HEIGHT / SQUINT_FACTOR;

    // affect mirror mode too
    if (chaos_check_if_patch_active(CHAOS_PATCH_MIRROR_MODE)) {
        chaos_viewport_width_target *= -1;
    }
}

void chs_squint_deinit(void) {
    chaos_viewport_width_target = SCREEN_WIDTH / 2;
    chaos_viewport_height_target = SCREEN_HEIGHT / 2;

    // affect mirror mode too
    if (chaos_check_if_patch_active(CHAOS_PATCH_MIRROR_MODE)) {
        chaos_viewport_width_target *= -1;
    }
}

