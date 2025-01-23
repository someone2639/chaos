#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/level_update.h"
#include "behavior_data.h"
#include "game/chaos/chaos.h"

void chs_update_wing_cap(void) {
    gMarioState->flags |= MARIO_WING_CAP | MARIO_CAP_ON_HEAD;
}