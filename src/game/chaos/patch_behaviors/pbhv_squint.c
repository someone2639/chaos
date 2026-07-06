#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "actors/group0.h"

#include "game/chaos/chaos.h"

#include "sounds.h"
#include "audio/external.h"
#include "game/camera.h"
#include "game/game_init.h"
#include "game/level_update.h"

#include "pbhv_viewport_mods.h"

f32 squint_room_pos[3];

void draw_room(float scaleXY) {
    Mtx trans;
    Mtx rot;
    Mtx scale;
    Mtx SR;
    Mtx *final = alloc_display_list(sizeof(Mtx));

    if (final == NULL) {
        return;
    }

    static float x = 160;
    static float y = 120;
    static float z = 160;
    // float x = gLakituState.pos[0];
    // float y = gLakituState.pos[1];
    // float z = gLakituState.pos[2];
    // float x = squint_room_pos[0];
    // float y = squint_room_pos[1];
    // float z = squint_room_pos[2];

    #define S16_TO_DEG(x) (((f32)(x) / 65536.0f) * 360.0f)

    guScale(&scale, scaleXY, scaleXY, 1.6f);
    guRotate(&rot, 0, 0, 1, 0);
    guTranslate(&trans, x, y, z);
    guMtxCatL(&scale, &rot, &SR);
    guMtxCatL(&SR, &trans, final);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(final++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF,G_RM_OPA_SURF2);
    gSPDisplayList(gDisplayListHead++, &squint_room_squint_room_mesh_layer_1);


    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    osSyncPrintf("%f %f %f, %f\n", x, y, z, S16_TO_DEG(gMarioState->area->camera->yaw));

    if (gPlayer1Controller->buttonDown & U_CBUTTONS) {
        z++;
    }
    if (gPlayer1Controller->buttonDown & D_CBUTTONS) {
        z--;
    }
}

void chs_squint_init(void) {
    chaos_viewport_width_target = SCREEN_WIDTH / SQUINT_FACTOR;
    chaos_viewport_height_target = SCREEN_HEIGHT / SQUINT_FACTOR;
    squint_room_scale_target = 1.6f;

    // affect mirror mode too
    if (chaos_check_if_patch_active(CHAOS_PATCH_MIRROR_MODE)) {
        chaos_viewport_width_target *= -1;
    }
}

void chs_squint_deinit(void) {
    chaos_viewport_width_target = SCREEN_WIDTH / 2;
    chaos_viewport_height_target = SCREEN_HEIGHT / 2;
    squint_room_scale_target = 5.5f;

    // affect mirror mode too
    if (chaos_check_if_patch_active(CHAOS_PATCH_MIRROR_MODE)) {
        chaos_viewport_width_target *= -1;
    }
}

