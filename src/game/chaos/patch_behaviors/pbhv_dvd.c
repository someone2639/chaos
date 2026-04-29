#include "engine/math_util.h"
#include "game/segment2.h"
#include "game/ingame_menu.h"
#include "game/game_init.h"
#include "engine/behavior_script.h"
#include "game/chaos_settings.h"
#include "game/main.h"

static Vec2f sDVDLogoPos;
static s16 sDVDLogoAngle;
static f32 sDVDColor; // This is an HSV angle so we only get bright colors

void chs_init_dvd(void) {
    sDVDLogoPos[0] = SCREEN_CENTER_X;
    sDVDLogoPos[1] = SCREEN_CENTER_Y;
    sDVDLogoAngle = RAND(0xFFFF);
    sDVDColor = random_float() * 360;
}

void dvd_bounce(s16 sideAngle) {
    sDVDLogoAngle = (sideAngle - (sDVDLogoAngle - sideAngle));
    sDVDColor = random_float() * 360;
}

void chs_update_dvd(void) {
    f32 velX = coss(sDVDLogoAngle) * 3.0f;
    f32 velY = sins(sDVDLogoAngle) * 3.0f;

    if(gConfig.widescreen & WIDE_SCREEN_UI) {
        velY *= 0.75f;
    }

    f32 x = sDVDLogoPos[0] + velX;
    f32 y = sDVDLogoPos[1] + velY;

    if (x > SCREEN_WIDTH - 32) {
        dvd_bounce(0x4000);
        x = SCREEN_WIDTH - 32;
    } else if (x < 32) {
        dvd_bounce(0xC000);
        x = 32;
    } else if (y > SCREEN_HEIGHT - 16) {
        dvd_bounce(0x0000);
        y = SCREEN_HEIGHT - 16;
    } else if (y < 16) {
        dvd_bounce(0x8000);
        y = 16;
    }

    sDVDLogoPos[0] = x;
    sDVDLogoPos[1] = y;
}

void draw_dvd_logo(void) {
    // Convert the color from hsv to rgb
    f32 r = 0;
    f32 g = 0;
    f32 b = 0;

    f32 h = sDVDColor;
    f32 s = 1.0f;
    f32 v = 1.0f;

    s16 i;
    f32 f, p, q, t;

    if(h == 360){
        h = 0;
    }
    else {
        h /= 60;
    }

    i = (s16)h;
    f = h - i;

    p = v * (1.0f - s);
    q = v * (1.0f - (s * f));
    t = v * (1.0f - (s * (1.0 - f)));

    switch (i) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        default:
            r = v;
            g = p;
            b = q;
            break;
    }

    u8 finalR = 255 * r;
    u8 finalG = 255 * g;
    u8 finalB = 255 * b;

    create_dl_translation_matrix(&gDisplayListHead, MENU_MTX_PUSH, sDVDLogoPos[0], sDVDLogoPos[1], 0.0f);
    if(gConfig.widescreen & WIDE_SCREEN_UI) {
        create_dl_scale_matrix(&gDisplayListHead, MENU_MTX_NOPUSH, 0.75f, 1.0f, 1.0f);
    }
    gDPSetPrimColor(gDisplayListHead++, 0, 0, finalR, finalG, finalB, 0xFF);
    gSPDisplayList(gDisplayListHead++, dvd_logo);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}