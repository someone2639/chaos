#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "course_table.h"
#include "sounds.h"
#include "audio/external.h"
#include "engine/behavior_script.h"
#include "game/debug.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/save_file.h"

void chs_act_input_lag(void) {
    bzero(chaosControllerLag, sizeof(chaosControllerLag));
}
