#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "behavior_data.h"
#include "game/mario.h"
#include "game/level_update.h"
#include "game/chaos/chaos.h"

u8 chs_cond_mario_big(void) {
    return (chaos_count_active_instances(CHAOS_PATCH_MARIO_BIG) < 3);
}

void chs_act_mario_big(void) {
    gMarioState->size += 0.125f;
}

// NOTE: Despite being an infinite effect, deactivation functions are still needed for Show Me Mercy to work correctly
// (or for cancelling patches, but not in this particular case).
void chs_deact_mario_big(void) {
    gMarioState->size -= 0.125f;
}

u8 chs_cond_mario_small(void) {
    return (chaos_count_active_instances(CHAOS_PATCH_MARIO_SMALL) < 3);
}

void chs_act_mario_small(void) {
    gMarioState->size -= 0.125f;
}

void chs_deact_mario_small(void) {
    gMarioState->size += 0.125f;
}
