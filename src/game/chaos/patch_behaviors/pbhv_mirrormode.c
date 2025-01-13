#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "sounds.h"
#include "audio/external.h"
#include "game/level_update.h"

extern u32 flipstate;

u8 chs_mq_check(void) {
    return TRUE;
}

void chs_mq_init(void) {
    flipstate = 0;
}

void chs_mq_deinit(void) {
    flipstate = 0;
}

