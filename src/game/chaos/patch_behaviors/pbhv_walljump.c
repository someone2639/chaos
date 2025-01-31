#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/level_update.h"
#include "behavior_data.h"
#include "game/chaos/chaos.h"

u8 chs_check_sticky_walljump(void) {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK));
}