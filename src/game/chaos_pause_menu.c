#include <ultra64.h>
#include <PR/gbi.h>

#include "segment2.h"
#include "print.h"
#include "gfx_dimensions.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "object_helpers.h"
#include "fasttext.h"
#include "level_update.h"
#include "debug.h"
#include "src/engine/math_util.h"
#include "audio/external.h"
#include "sm64.h"
#include "save_file.h"
#include "chaos_menus.h"
#include "chaos_pause_menu.h"
#include "main.h"

struct ChaosPauseMenu sChaosPauseMenu;
struct ChaosPauseMenu *gChaosPauseMenu = &sChaosPauseMenu;

void scroll_settings_panel() {
    int i = 0;
	int count = 4;
	int width = 64 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(opt_bg_opt_mesh_mesh_vtx_0);

	deltaX = (int)(0.1 * 0x20) % width;
	deltaY = (int)(0.1 * 0x20) % height;

	if (absi(currentX) > width) {
		deltaX -= (int)(absi(currentX) / width) * width * signum_positive(deltaX);
	}
	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[0] += deltaX;
		vertices[i].n.tc[1] += deltaY;
	}
	currentX += deltaX;	currentY += deltaY;
}

void render_settings_panel() {
    scroll_settings_panel();

    Mtx *transMtx = alloc_display_list(sizeof(Mtx) * 2);
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    f32 x = 42;
    f32 y = 52;

    u8 aspRatCol43;
    u8 aspRatCol169;
    u8 musColOn;
    u8 musColOff;
    s32 cursorX = (gChaosPauseMenu->settingsMenu.selectedMenuIndex % 2) ? 0 : -30;
    s32 cursorY = (gChaosPauseMenu->settingsMenu.selectedMenuIndex < 2) ? 5 : -35;

    if(gConfig.widescreen) {
        aspRatCol43 = 0x7F;
        aspRatCol169 = 0xFF;
    } else {
        aspRatCol43 = 0xFF;
        aspRatCol169 = 0x7F;
    }

    if(gConfig.disableBGMusic) {
        musColOn = 0x7F;
        musColOff = 0xFF;
    } else {
        musColOn = 0xFF;
        musColOff = 0x7F;
    }

    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, opt_bg_opt_mesh_mesh);

    slowtext_setup_ortho_rendering(FT_FONT_SMALL_BOLD);
    slowtext_draw_ortho_text(0, 25, "Aspect\nRatio:", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(0, -25, "Music:", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_setup_ortho_rendering(FT_FONT_SMALL_THIN);
    slowtext_draw_ortho_text(-15, 0, "4:3", FT_FLAG_ALIGN_CENTER, aspRatCol43, aspRatCol43, aspRatCol43, 0xFF);
    slowtext_draw_ortho_text(15, 0, "16:9", FT_FLAG_ALIGN_CENTER, aspRatCol169, aspRatCol169, aspRatCol169, 0xFF);
    slowtext_draw_ortho_text(-15, -40, "On", FT_FLAG_ALIGN_CENTER, musColOn, musColOn, musColOn, 0xFF);
    slowtext_draw_ortho_text(15, -40, "Off", FT_FLAG_ALIGN_CENTER, musColOff, musColOff, musColOff, 0xFF);
    slowtext_finished_rendering();

    //Cursor
    guTranslate(transMtx, cursorX, cursorY, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    guScale(scaleMtx, 0.5f, 0.5f, 1.0f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gDPSetEnvColor(gDisplayListHead++, 0xFF, 0xFF, 0xFF, 0xFF);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);


    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void handle_settings_inputs() {
    u32 stickDir = menu_update_joystick_dir(&gChaosPauseMenu->settingsMenu);
    s32 prevSelection = gChaosPauseMenu->settingsMenu.selectedMenuIndex;
    s32 selection = prevSelection;
    s32 pressedUpDown = ((gPlayer1Controller->buttonPressed & D_JPAD) || (stickDir == MENU_JOYSTICK_DIR_DOWN) || 
        (gPlayer1Controller->buttonPressed & U_JPAD) || (stickDir == MENU_JOYSTICK_DIR_UP));
    s32 pressedLeftRight = ((gPlayer1Controller->buttonPressed & (L_JPAD | R_JPAD)) || stickDir & (MENU_JOYSTICK_DIR_LEFT | MENU_JOYSTICK_DIR_RIGHT));

    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        switch(selection) {
            case 0:
                gConfig.widescreen = 0;
                break;
            case 1:
                gConfig.widescreen = 1;
                break;
            case 2:
                gConfig.disableBGMusic = 0;
                break;
            case 3:
                gConfig.disableBGMusic = 1;
                break;
        }
        save_file_set_widescreen_mode(gConfig.widescreen);
        save_file_set_bg_music(gConfig.disableBGMusic);
    } else if (gPlayer1Controller->buttonPressed & (B_BUTTON | L_TRIG)) {
        gChaosPauseMenu->settingsMenu.flags &= ~CHAOS_SETTINGS_ACTIVE;
        gPlayer1Controller->buttonPressed &= ~(B_BUTTON | L_TRIG);
    } else if(pressedUpDown) {
        selection += 2;
        if(selection > 3) {
            selection = prevSelection - 2;
        }
    } else if (pressedLeftRight) {
        selection++;
        if(selection > 3 || !(selection % 2)) {
            selection = prevSelection - 1;
        }
    }

    if(selection != prevSelection) {
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
        gChaosPauseMenu->settingsMenu.selectedMenuIndex = selection;
    }
}