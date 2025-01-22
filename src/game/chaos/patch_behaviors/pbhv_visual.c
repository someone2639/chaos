#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "course_table.h"
#include "sounds.h"
#include "audio/external.h"
#include "game/area.h"
#include "game/debug.h"
#include "game/level_update.h"

u8 chs_cond_no_zbuffer(void)       { return !chaos_check_if_patch_active(CHAOS_PATCH_INVERTED_Z_BUFFER); }
u8 chs_cond_inverted_zbuffer(void) { return !chaos_check_if_patch_active(CHAOS_PATCH_NO_Z_BUFFER      ); }

u8 chs_cond_decreased_fov(void)       { return !chaos_check_if_patch_active(CHAOS_PATCH_INCREASED_FOV); }
u8 chs_cond_increased_fov(void)       { return !chaos_check_if_patch_active(CHAOS_PATCH_DECREASED_FOV); }
