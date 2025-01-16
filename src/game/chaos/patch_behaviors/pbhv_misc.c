#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "game/chaos/chaos.h"

u8 chs_cond_one_hit_wonder(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_RANDOM_SHOCK) && !chaos_check_if_patch_active(CHAOS_PATCH_RANDOM_BURN));
}