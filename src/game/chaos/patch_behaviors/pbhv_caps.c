#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/level_update.h"
#include "behavior_data.h"
#include "game/chaos/chaos.h"
#include "game/save_file.h"
#include "buffers/buffers.h"
#include "engine/math_util.h"

void chs_update_wing_cap(void) {
    gMarioState->flags |= MARIO_WING_CAP | MARIO_CAP_ON_HEAD;
}

void chs_act_remove_cap(void) {
    s32 rand = RAND(3);

    switch(rand) {
        case 0:
            save_file_set_flags(SAVE_FLAG_CAP_ON_KLEPTO);
            break;
        case 1:
            save_file_set_flags(SAVE_FLAG_CAP_ON_MR_BLIZZARD);
            break;
        case 2:
        default:
            save_file_set_flags(SAVE_FLAG_CAP_ON_UKIKI);
            break;
    }

    gMarioState->flags &= ~MARIO_CAP_ON_HEAD;
}

u8 chs_cond_remove_cap(void) {
    s32 flags = save_file_get_flags();
    return !(flags & (SAVE_FLAG_CAP_ON_GROUND | SAVE_FLAG_CAP_ON_KLEPTO | SAVE_FLAG_CAP_ON_MR_BLIZZARD | SAVE_FLAG_CAP_ON_UKIKI));
}

u8 chs_cond_disable_caps(void) {
    s32 flags = save_file_get_flags();
    return (flags & (SAVE_FLAG_HAVE_WING_CAP | SAVE_FLAG_HAVE_METAL_CAP | SAVE_FLAG_HAVE_VANISH_CAP));
}