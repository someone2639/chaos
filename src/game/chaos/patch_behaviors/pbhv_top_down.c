#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "behavior_data.h"
#include "game/chaos/chaos.h"

s16 prevMode = CAMERA_MODE_NONE;
u32 topdown_semaphore = 0;

extern void transition_to_camera_mode(struct Camera *c, s16 newMode, s16 numFrames);

void chs_init_top_down(void) {
    prevMode = gCamera->mode;
}

void chs_deinit_top_down(void) {
    topdown_semaphore = 1;
}
