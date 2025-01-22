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
#include "chaos/chaos.h"
#include "patch_selection_ui.h"

#define PATCH_LIST_SIZE     6
struct ChaosPauseMenu sChaosPauseMenu = {.chaosListStart = 0};
struct ChaosPauseMenu *gChaosPauseMenu = &sChaosPauseMenu;

#define MINI_CARD_STRING_WIDTH 94

void scroll_mini_patch_cards() {
    int i = 0;
	int count = 4;
	int width = 128 * 0x20;
	int height = 64 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(patch_bg_s_mesh_s_mesh_vtx_0);

	deltaX = (int)(-0.25f * 0x20) % width;
	deltaY = (int)(-0.25f * 0x20) % height;

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

void draw_mini_patch_card(f32 x, f32 y, struct ChaosActiveEntry *patch) {
    const struct ChaosPatch *patchInfo = &gChaosPatches[patch->id];
    const char* patchName = patchInfo->name;
    char drawName[48];
    char timerText[4];
    f32 nameY;
    s32 lines, length;
    
    u8 effectR = sEffectColors[patchInfo->effectType][0];
    u8 effectG = sEffectColors[patchInfo->effectType][1];
    u8 effectB = sEffectColors[patchInfo->effectType][2];

    //Center name if it's only one line long
    fasttext_compute_print_text_with_line_breaks(FT_FONT_SMALL_THIN, MINI_CARD_STRING_WIDTH, &lines, &length, drawName, patchName);
    nameY = (lines == 1) ? -10 : -3;
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));

    gDPSetPrimColor(gDisplayListHead++, 0, 0, effectR, effectG, effectB, 0xFF);
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, patch_bg_s_mesh_s_mesh);

    if(patchInfo->durationType == CHAOS_DURATION_STARS || patchInfo->durationType == CHAOS_DURATION_USE_COUNT) {
        sprintf(timerText, "%d", patch->remainingDuration);
        gSPDisplayList(gDisplayListHead++, patch_use_type_start);
        draw_patch_type(38, 0, patchInfo->durationType);
        gSPDisplayList(gDisplayListHead++, patch_use_type_end);
    } else if (patchInfo->durationType == CHAOS_DURATION_INFINITE) {
        sprintf(timerText, "`"); // Infinity symbol
        gSPDisplayList(gDisplayListHead++, patch_use_type_start);
        draw_patch_type(38, 0, patchInfo->durationType);
        gSPDisplayList(gDisplayListHead++, patch_use_type_end);
    } else {
        sprintf(timerText, '\0');
    }

    slowtext_setup_ortho_rendering(FT_FONT_SMALL_THIN);
    slowtext_draw_ortho_text(-62, nameY, drawName, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_setup_ortho_rendering(FT_FONT_OUTLINE);
    slowtext_draw_ortho_text(46, -10, timerText, FT_FLAG_ALIGN_LEFT, 0xD0, 0xC4, 0x00, 0xFF);
    slowtext_finished_rendering();
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_active_patches() {
    if (gChaosActiveEntryCount == NULL) {
        return;
    }
    scroll_mini_patch_cards();

    f32 cardX = SCREEN_WIDTH - 74;
    s32 selection = gChaosPauseMenu->activePatchesMenu.selectedMenuIndex;
    s32 numPatches = *gChaosActiveEntryCount;
    s32 listStart = gChaosPauseMenu->chaosListStart;
    s32 listEnd = (numPatches > PATCH_LIST_SIZE) ? (listStart + PATCH_LIST_SIZE) : numPatches;

    for(int i = listStart; i < listEnd; i++) {
        draw_mini_patch_card(cardX - (20 * (i == selection)), SCREEN_HEIGHT - 35 - (35 * (i - listStart)), &gChaosActiveEntries[i]);
    }
}

void update_active_patch_list_bounds() {
    s32 selection = gChaosPauseMenu->activePatchesMenu.selectedMenuIndex;
    s32 start = gChaosPauseMenu->chaosListStart;
    s32 numPatches = *gChaosActiveEntryCount;
    s32 end = start + PATCH_LIST_SIZE;
    if(PATCH_LIST_SIZE >= numPatches) {
        return;
    }

    if(selection > end - 2) {
        start = selection - PATCH_LIST_SIZE + 2;
    } else if (selection < start + 1) {
        start = selection - 1;
    }

    end = start + PATCH_LIST_SIZE;

    if(end > numPatches) {
        start = numPatches - PATCH_LIST_SIZE;
    } else if (start < 0) {
        start = 0;
    }

    gChaosPauseMenu->chaosListStart = start;

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

void handle_active_patches_inputs() {
    if(!gChaosActiveEntryCount) {
        return;
    }

    u32 stickDir = menu_update_joystick_dir(&gChaosPauseMenu->activePatchesMenu);
    s32 prevSelection = gChaosPauseMenu->activePatchesMenu.selectedMenuIndex;
    s32 selection = prevSelection;

    if(gPlayer1Controller->buttonPressed & (R_TRIG | B_BUTTON | A_BUTTON | START_BUTTON)) {
        gChaosPauseMenu->activePatchesMenu.flags &= ~ACTIVE_PATCHES_MENU_ACTIVE;
        gPlayer1Controller->buttonPressed &= ~R_TRIG;
    } else if(gPlayer1Controller->buttonPressed & U_JPAD || stickDir & MENU_JOYSTICK_DIR_UP) {
        selection--;
    } else if (gPlayer1Controller->buttonPressed & D_JPAD || stickDir & MENU_JOYSTICK_DIR_DOWN) {
        selection++;
    }

    if(selection < 0) {
        selection = *gChaosActiveEntryCount - 1;
    } else if (selection >= *gChaosActiveEntryCount) {
        selection = 0;
    }

    if(selection != prevSelection) {
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
        gChaosPauseMenu->activePatchesMenu.selectedMenuIndex = selection;
        update_active_patch_list_bounds();
    }
}

void handle_settings_inputs() {
    u32 stickDir = menu_update_joystick_dir(&gChaosPauseMenu->settingsMenu);
    s32 prevSelection = gChaosPauseMenu->settingsMenu.selectedMenuIndex;
    s32 selection = prevSelection;
    s32 pressedUpDown = ((gPlayer1Controller->buttonPressed & (D_JPAD | U_JPAD)) || stickDir & (MENU_JOYSTICK_DIR_DOWN | MENU_JOYSTICK_DIR_UP));
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