#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "behavior_data.h"
#include "game/mario.h"
#include "game/level_update.h"
#include "game/chaos/chaos.h"

void chs_act_mario_big(void) {
    gMarioState->size += 0.125f;
}

u8 chs_cond_mario_big(void) {
    return(chaos_count_active_instances(CHAOS_PATCH_MARIO_BIG) < 4);
}

void chs_act_mario_small(void) {
    gMarioState->size -= 0.125f;
}

u8 chs_cond_mario_small(void) {
    return(chaos_count_active_instances(CHAOS_PATCH_MARIO_SMALL) < 4);
}
