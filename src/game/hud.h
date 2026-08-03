#ifndef HUD_H
#define HUD_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>

enum PowerMeterAnimation {
    POWER_METER_HIDDEN,
    POWER_METER_EMPHASIZED,
    POWER_METER_DEEMPHASIZING,
    POWER_METER_HIDING,
    POWER_METER_VISIBLE
};

enum CameraHUDLut {
    GLYPH_CAM_CAMERA,
    GLYPH_CAM_MARIO_HEAD,
    GLYPH_CAM_LAKITU_HEAD,
    GLYPH_CAM_FIXED,
    GLYPH_CAM_ARROW_UP,
    GLYPH_CAM_ARROW_DOWN,
    GLYPH_CAM_45_DEGREES,
};
void draw_sprite(Gfx **dl, const void *texture, s32 dlImgFormat, s32 dlImgSize, s32 bilerp,
        u32 textureWidth, u32 textureHeight, f32 x, f32 y, f32 displayWidth, f32 displayHeight);
void set_hud_camera_status(s16 status);
void render_hud(void);
void render_hud_tex_lut(s32 x, s32 y, u8 *texture);

#endif // HUD_H
