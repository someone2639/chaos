#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "sounds.h"
#include "audio/external.h"
#include "game/level_update.h"

#include "pbhv_viewport_mods.h"

void chs_mq_init(void) {
    chaos_viewport_width_target = -SCREEN_WIDTH / 2;

    if (chaos_check_if_patch_active(CHAOS_PATCH_SQUINT_MODE)) {
        chaos_viewport_width_target = -SCREEN_WIDTH / SQUINT_FACTOR;
    }
}

void chs_mq_deinit(void) {
    chaos_viewport_width_target = SCREEN_WIDTH / 2;

    if (chaos_check_if_patch_active(CHAOS_PATCH_SQUINT_MODE)) {
        chaos_viewport_width_target = SCREEN_WIDTH / SQUINT_FACTOR;
    }
}

