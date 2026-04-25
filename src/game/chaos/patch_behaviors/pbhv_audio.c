#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "audio/heap.h"
#include "game/chaos/chaos.h"
#include "game/emutest.h"
#include "game/level_update.h"
#include "game/area.h"
#include "game/main.h"
#include "game/object_helpers.h"
#include "game/save_file.h"
#include "course_table.h"
#include "behavior_data.h"
#include "engine/surface_collision.h"

void chs_act_reverb(void) { init_reverb_us(1U << 31); }
void chs_deact_reverb(void) { init_reverb_us(1U << 31); }

u8 chs_cond_randomized_music(void) {
    return (!gConfig.disableBGMusic);
}

u8 chs_cond_chill_out(void) {
    return (!gConfig.disableBGMusic);
}

u8 chs_cond_mad_musical_mess(void) {
    return (!gConfig.disableBGMusic);
}
