#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "sounds.h"
#include "audio/external.h"
#include "game/level_update.h"

extern s32 fliptarget;

u8 chs_mq_check(void) {
    return TRUE;
}

void chs_mq_init(void) {
    fliptarget = -SCREEN_WIDTH / 2;
}

void chs_mq_deinit(void) {
    fliptarget = SCREEN_WIDTH / 2;
}

