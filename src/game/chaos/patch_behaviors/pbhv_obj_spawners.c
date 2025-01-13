#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/level_update.h"
#include "behavior_data.h"
#include "game/object_helpers.h"

void chs_area_init_green_demon(void) {
    spawn_object_abs_with_rot(gMarioState->marioObj, 0, MODEL_GREEN_DEMON, bhvGreenDemon,
                            gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2], 0, 0, 0);
}