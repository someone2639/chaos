#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "actors/group0.h"

#include "game/chaos/chaos.h"

#include "sounds.h"
#include "audio/external.h"
#include "engine/math_util.h"
#include "game/camera.h"
#include "game/game_init.h"
#include "game/ingame_menu.h"
#include "game/level_update.h"

#include "pbhv_viewport_mods.h"

static float internal_scale = 0;

static void betah(int next_state) {
    static int cur_state = 0;
    u8 *betah_tex = segmented_to_virtual(squint_room_betah_rgba_ci8);
    u8 *betah_pal = segmented_to_virtual(squint_room_betah_rgba_pal_rgba16);

    u8 *normal_tex = segmented_to_virtual(squint_room_inside_texture_ci8);
    u8 *normal_pal = segmented_to_virtual(squint_room_inside_texture_pal_rgba16);

    u8 *buffer_tex = segmented_to_virtual(squint_room_inside_buffer_ci8);
    u8 *buffer_pal = segmented_to_virtual(squint_room_inside_buffer_pal_rgba16);

    if (next_state != cur_state) {
        next_state = cur_state;

        osWritebackDCacheAll();
        switch (next_state) {
            case 0:
                dma_read(buffer_tex, normal_tex, normal_tex + 0x800);
                osWritebackDCacheAll();
                dma_read(buffer_pal, normal_pal, normal_pal + 0x1A6);
                break;
            case 1:
                dma_read(buffer_tex, betah_tex, betah_tex + 0x800);
                osWritebackDCacheAll();
                dma_read(buffer_pal, betah_pal, betah_pal + 0x1CC);
                break;
        }
        osWritebackDCacheAll();
    }
}

static void ortho(Gfx **dl) {
    Gfx *dlHead = *dl;
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));

    if (matrix == NULL) {
        return;
    }

    create_dl_identity_matrix(&dlHead);

    guOrtho(matrix, 0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -600.0f, 200.0f, 1.0f);

    // Should produce G_RDPHALF_1 in Fast3D
    gSPPerspNormalize(dlHead++, 0xFFFF);

    gSPMatrix(dlHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);

    *dl = dlHead;
}

#define BUTTON_STICK_TRANSITION_SPEED 45.0f

static void mtxTransformRPY(Mtx *m, Vec3f translate, Vec3f rotateRPY, Vec3f scale) {
    Mtx transM;
    Mtx rotM;
    Mtx scaleM;
    Mtx SR;

    translate[1] -= ((internal_scale - 1.6f) * BUTTON_STICK_TRANSITION_SPEED);

    guScale(&scaleM, scale[0], scale[1], scale[2]);
    guRotateRPY(&rotM, rotateRPY[0], rotateRPY[1], rotateRPY[2]);
    guTranslate(&transM, translate[0], translate[1], translate[2]);
    guMtxCatL(&scaleM, &rotM, &SR);
    guMtxCatL(&SR, &transM, m);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(m), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
}

static void mtxTransformStick(Mtx *m, Vec3f translate, Vec2f stick, Vec3f scale) {
    Mtx transM;
    Mtx rotM;
    Mtx scaleM;
    Mtx SR;

    translate[1] -= ((internal_scale - 1.6f) * BUTTON_STICK_TRANSITION_SPEED);

    float totalStick = vec2_mag(stick) / 80.0f * 40.0f;
    Vec3f axis = {stick[0], 0, -stick[1]};
    vec3_normalize(axis);

    guScale(&scaleM, scale[0], scale[1], scale[2]);
    guRotate(&rotM, totalStick, axis[0], axis[1], axis[2]);
    guTranslate(&transM, translate[0], translate[1], translate[2]);
    guMtxCatL(&scaleM, &rotM, &SR);
    guMtxCatL(&SR, &transM, m);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(m), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
}

#define btn(button, val) \
    ((gPlayer1Controller->buttonDown & (button))) ? \
        ((val) - 2) : (val)

static void draw_n64_controls(void) {
    void *buttonDL = squint_room_button_squint_room_button_mesh_layer_1;
    void *stickDL = squint_room_stick_squint_room_stick_mesh_layer_1;
    void *dpadDL = squintroom_dpad_squintroom_dpad_mesh_layer_1;
    Mtx *buttonMtxs = alloc_display_list(sizeof(Mtx) * 10);

    u32 curMtx = 0;

    // A
    gDPSetEnvColor(gDisplayListHead++, 4, 64, 191, 0xFF);
    mtxTransformRPY(&buttonMtxs[curMtx++],
        (Vec3f){188.0, btn(A_BUTTON, 47.0), btn(A_BUTTON, 390)},
        (Vec3f){70, 0, 0},
        (Vec3f){1.0f, 1.0f, 1.0f}
    );
    gSPDisplayList(gDisplayListHead++, buttonDL);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    // B
    gDPSetEnvColor(gDisplayListHead++, 0, 153, 46, 0xFF);
    mtxTransformRPY(&buttonMtxs[curMtx++],
        (Vec3f){179.0, btn(B_BUTTON, 54.0), btn(B_BUTTON, 380)},
        (Vec3f){70, 0, 0},
        (Vec3f){1.0f, 1.0f, 1.0f}
    );
    gSPDisplayList(gDisplayListHead++, buttonDL);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    // C buttons
    #define C_BUTTON_SCALE 0.6f
    gDPSetEnvColor(gDisplayListHead++, 253, 200, 2, 0xFF);
    mtxTransformRPY(&buttonMtxs[curMtx++],
        (Vec3f){211, btn(U_CBUTTONS, 64), btn(U_CBUTTONS, 365)},
        (Vec3f){70, 0, 0},
        (Vec3f){C_BUTTON_SCALE, C_BUTTON_SCALE, C_BUTTON_SCALE}
    );
    gSPDisplayList(gDisplayListHead++, buttonDL);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    mtxTransformRPY(&buttonMtxs[curMtx++],
        (Vec3f){203, btn(L_CBUTTONS, 58), btn(L_CBUTTONS, 372)},
        (Vec3f){70, 0, 0},
        (Vec3f){C_BUTTON_SCALE, C_BUTTON_SCALE, C_BUTTON_SCALE}
    );
    gSPDisplayList(gDisplayListHead++, buttonDL);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    mtxTransformRPY(&buttonMtxs[curMtx++],
        (Vec3f){219, btn(R_CBUTTONS, 58), btn(R_CBUTTONS, 372)},
        (Vec3f){70, 0, 0},
        (Vec3f){C_BUTTON_SCALE, C_BUTTON_SCALE, C_BUTTON_SCALE}
    );
    gSPDisplayList(gDisplayListHead++, buttonDL);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    mtxTransformRPY(&buttonMtxs[curMtx++],
        (Vec3f){211, btn(D_CBUTTONS, 52), btn(D_CBUTTONS, 380)},
        (Vec3f){70, 0, 0},
        (Vec3f){C_BUTTON_SCALE, C_BUTTON_SCALE, C_BUTTON_SCALE}
    );
    gSPDisplayList(gDisplayListHead++, buttonDL);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    // start button
    gDPSetEnvColor(gDisplayListHead++, 245, 13, 35, 0xFF);
    mtxTransformRPY(&buttonMtxs[curMtx++],
        (Vec3f){160, btn(START_BUTTON, 65), btn(START_BUTTON, 360)},
        (Vec3f){70, 0, 0},
        (Vec3f){1.0f, 1.0f, 1.0f}
    );
    gSPDisplayList(gDisplayListHead++, buttonDL);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    // dpad
    int dpadXRot = 0;
    int dpadYRot = 0;
    if (gPlayer1Controller->buttonDown & L_JPAD) dpadXRot = -20;
    if (gPlayer1Controller->buttonDown & R_JPAD) dpadXRot = 20;
    if (gPlayer1Controller->buttonDown & U_JPAD) dpadYRot = -20;
    if (gPlayer1Controller->buttonDown & D_JPAD) dpadYRot = 20;
    mtxTransformStick(&buttonMtxs[curMtx++],
        (Vec3f){118.0, 56.0, 254.0},
        (Vec2f){dpadYRot + 80, dpadXRot},
        (Vec3f){1.0f, 1.0f, 1.0f}
    );
    gSPDisplayList(gDisplayListHead++, dpadDL);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    // stick
    mtxTransformStick(&buttonMtxs[curMtx++],
        (Vec3f){160, 21, 281.0},
        (Vec2f){-gPlayer1Controller->stickY + 40, gPlayer1Controller->stickX},
        (Vec3f){1.0f, 1.0f, 1.0f}
    );
    gSPDisplayList(gDisplayListHead++, stickDL);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    u8 tiltCart = chaos_check_if_patch_active(CHAOS_PATCH_CARTRIDGE_TILT);
    mtxTransformRPY(&buttonMtxs[curMtx++],
        (Vec3f){66, 76, 13},
        (Vec3f){0, 0, tiltCart ? 20 : 0},
        (Vec3f){1.0f, 1.0f, 1.0f}
    );
    gSPDisplayList(gDisplayListHead++, squint_room_cart_squint_room_cart_mesh_layer_3_with_revert);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void draw_room(float scaleXY) {
    // betah(chaos_check_if_patch_active(CHAOS_PATCH_BETA));
    // betah(gPlayer1Controller->buttonDown & A_BUTTON);

    internal_scale = scaleXY;
    Mtx *baseScale = alloc_display_list(sizeof(Mtx));
    Mtx *final = alloc_display_list(sizeof(Mtx));

    if (baseScale == NULL) {
        return;
    }
    if (final == NULL) {
        return;
    }

    ortho(&gDisplayListHead);
    gSPSetGeometryMode(gDisplayListHead++, G_ZBUFFER);

    // Hack to make the final scene not transform like the buttons
    internal_scale = 1.6f;
    mtxTransformRPY(final,
        (Vec3f){160, 120, 160},
        (Vec3f){0, 0, 0},
        (Vec3f){scaleXY, scaleXY, 1.6f}
    );
    internal_scale = scaleXY;

    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF,G_RM_OPA_SURF2);
    gSPDisplayList(gDisplayListHead++, &squint_room_squint_room_mesh_layer_1_with_revert);

    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_ZB_OPA_SURF,G_RM_AA_ZB_OPA_SURF2);
    gSPDisplayList(gDisplayListHead++, &squint_room_squint_room_mesh_layer_2_with_revert);
    gSPDisplayList(gDisplayListHead++, &squint_room_squint_room_mesh_layer_3_with_revert);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_ZB_OPA_SURF,G_RM_AA_ZB_OPA_SURF2);
    draw_n64_controls();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    gSPPopMatrix(gDisplayListHead++, G_MTX_PROJECTION);
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

