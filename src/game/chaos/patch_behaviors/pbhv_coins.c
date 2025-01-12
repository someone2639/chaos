#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "sounds.h"
#include "audio/external.h"
#include "game/debug.h"
#include "game/level_update.h"

#define TIMELIMIT (30 * 30)

u8 chs_double_coins_under_30s(void) {
    struct ChaosActiveEntry *entry;

    if (!chaos_check_if_patch_active(CHAOS_PATCH_DOUBLE_COINS)) {
        return FALSE;
    }

    if (!chaos_find_first_active_patch(CHAOS_PATCH_DOUBLE_COINS, &entry)) {
        assert(FALSE, "chs_lvlinit_double_coins:\nActive patch could not be found!");
    }

    return (entry->frameTimer < TIMELIMIT);
}

void chs_lvlinit_double_coins(void) {
    struct ChaosActiveEntry *entry;

    if (!chaos_find_first_active_patch(CHAOS_PATCH_DOUBLE_COINS, &entry)) {
        assert(FALSE, "chs_lvlinit_double_coins:\nActive patch could not be found!");
    }

    entry->frameTimer = 0;
}
