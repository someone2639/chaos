#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "game/chaos/chaos.h"
#include "game/level_update.h"
#include "engine/math_util.h"

/*
    Push Backwards
*/

u8 chs_cond_push_back(void) {
    return (chaos_count_active_instances(CHAOS_PATCH_PUSH_BACK) < 3);
}

void chs_update_push_back(void) {
    if (gMarioState->action & (ACT_FLAG_INTANGIBLE | ACT_GROUP_CUTSCENE)) {
        return;
    }
    
    s16 pushAngle = gMarioState->faceAngle[1] + 0x8000;
    f32 speed = 8.0f;

    gMarioState->pos[0] += speed * sins(pushAngle);
    gMarioState->pos[2] += speed * coss(pushAngle);
}