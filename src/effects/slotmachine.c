#include "actors/group0.h"
#include "game/ingame_menu.h"
#include "game/game_init.h"

// to get slot i: 30 + (60 * i)
// i = 1 for blue coin

void slot_draw(int i, int x, int y) {
    Mtx trans;
    Mtx rot;
    Mtx scale;
    Mtx SR;
    Mtx *final = alloc_display_list(sizeof(Mtx));
    static u32 rotation = 0;

    if (final == NULL) {
        return;
    }

    guScale(&scale, 0.25f, 0.25f, 0.25f);
    guRotate(&rot, (60 * i) + (gGlobalTimer * 16), 1, 0, 0);
    guTranslate(&trans, x, y, -40);
    guMtxCatL(&scale, &rot, &SR);
    guMtxCatL(&SR, &trans, final);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(final++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);


    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_ZB_OPA_SURF,G_RM_ZB_OPA_SURF2);
    gSPSetGeometryMode(gDisplayListHead++, G_ZBUFFER);
    gSPDisplayList(gDisplayListHead++, &slotwheel_slotwheel_mesh_layer_1);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

