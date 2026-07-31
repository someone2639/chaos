#include "engine/math_util.h"
#include "game/area.h"
#include "game/segment2.h"
#include "game/ingame_menu.h"
#include "game/level_update.h"
#include "game/game_init.h"
#include "engine/behavior_script.h"
#include "game/chaos_settings.h"
#include "game/main.h"
#include "game/colors.h"

static Vec2f sDVDLogoPos;
static s16 sDVDLogoAngle;
static HSV sDVDColor;

static void dvd_bounce(s16 sideAngle) {
    sDVDLogoAngle = (sideAngle - (sDVDLogoAngle - sideAngle));
    sDVDColor.h = random_float() * 360;
}

static void update_dvd_position(void) {
    f32 velX = coss(sDVDLogoAngle) * 3.0f;
    f32 velY = sins(sDVDLogoAngle) * 3.0f;

    if(gConfig.widescreen & WIDE_SCREEN_UI) {
        velX *= (4.0f / 3.0f) * (9.0f / 16.0f); // 0.75f
    }

    f32 x = sDVDLogoPos[0] + velX;
    f32 y = sDVDLogoPos[1] + velY;

    if (x > SCREEN_WIDTH - 32) {
        dvd_bounce(0x4000);
        x = SCREEN_WIDTH - 32;
    } else if (x < 32) {
        dvd_bounce(0xC000);
        x = 32;
    }
    if (y > SCREEN_HEIGHT - 16) {
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
    update_dvd_position();

    if (!check_moving_play_mode(sCurrPlayMode) || gInActSelect) {
        return;
    }

    RGBA col = HSV_to_RGB(sDVDColor);

    create_dl_translation_matrix(&gDisplayListHead, MENU_MTX_PUSH, sDVDLogoPos[0], sDVDLogoPos[1], 0.0f);
    if(gConfig.widescreen & WIDE_SCREEN_UI) {
        create_dl_scale_matrix(&gDisplayListHead, MENU_MTX_NOPUSH, 0.75f, 1.0f, 1.0f);
    }
    gDPSetPrimColor(gDisplayListHead++, 0, 0, col.c.r, col.c.g, col.c.b, 0xFF);
    gSPDisplayList(gDisplayListHead++, dvd_logo);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void chs_init_dvd(void) {
    sDVDLogoPos[0] = random_u16() % (SCREEN_WIDTH - 32);
    sDVDLogoPos[1] = random_u16() % (SCREEN_HEIGHT - 16);
    sDVDLogoAngle = 0xD874;
    sDVDColor.h = random_float() * 360;
    sDVDColor.s = 1.0f;
    sDVDColor.v = 1.0f;
    sDVDColor.a = 1.0f;

    s32 rand = random_u16() % 4;
    if (rand < 2) {
        dvd_bounce(0x0000);
    }
    if (rand % 2 == 0) {
        dvd_bounce(0x4000);
    }
}