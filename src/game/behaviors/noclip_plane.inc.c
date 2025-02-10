#include "engine/surface_collision.h"

void bhv_noclip_plane_loop(void) {
    o->oPosX = gMarioState->pos[0];
    o->oPosZ = gMarioState->pos[2];
    o->oPosY = FLOOR_LOWER_LIMIT + 50;
    if(!chaos_check_if_patch_active(CHAOS_PATCH_NOCLIP)) {
        obj_mark_for_deletion(o);
    }
}