#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "sounds.h"
#include "audio/external.h"
#include "game/level_update.h"

#define GRAVITY 0.375f

u8 chs_cond_gravity_decrease(void) { return !chaos_check_if_patch_active(CHAOS_PATCH_GRAVITY_INCREASE); }
u8 chs_cond_gravity_increase(void) { return !chaos_check_if_patch_active(CHAOS_PATCH_GRAVITY_DECREASE); }

void chs_act_gravity_decrease(void) { gMarioState->gravity -= GRAVITY; }
void chs_act_gravity_increase(void) { gMarioState->gravity += GRAVITY; }

void chs_deact_gravity_decrease(void) { gMarioState->gravity += GRAVITY; }
void chs_deact_gravity_increase(void) { gMarioState->gravity -= GRAVITY; }
