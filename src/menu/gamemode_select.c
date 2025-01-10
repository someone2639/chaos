#include <ultra64.h>
#include <PR/gbi.h>

#include "game/object_helpers.h"
#include "gamemode_select.h"
#include "game/segment2.h"
#include "src/engine/math_util.h"
#include "game/game_init.h"
#include "game/fasttext.h"
#include "game/ingame_menu.h"

struct GamemodeSelectMenu sGamemodeSelectMenu;

const char *sGMSelectDescriptions[] = {
    "Select game difficulty",
    "Select game mode",
    "Patch cards will be more favorable.\nCards will feature stronger positive effects.",
    "Patch cards will be evenly balanced.\nCards will have equal positive and negative effects.",
    "Patch cards will be more punishing.\nCards will feature stronger negative effects.",
    "Normal Super Mario 64 rules. Running out of lives\nleads to a game over. Collect 1-up mushrooms to\nincrease your life counter.",
    "Enables permadeath. Running out of lives deletes the\nsave file. 1-up mushrooms are removed, and lives can\nonly be increased by collecting yellow stars."
};

void init_gamemode_select() {
    sGamemodeSelectMenu.menu.flags = 0;
    sGamemodeSelectMenu.menu.menuState = GM_SELECT_STATE_DEFAULT;
    sGamemodeSelectMenu.menu.selectedMenuIndex = 0;
    sGamemodeSelectMenu.menu.framesSinceLastStickInput = 0;
    sGamemodeSelectMenu.menu.lastStickDir = MENU_JOYSTICK_DIR_NONE;
    sGamemodeSelectMenu.menu.animTimer = 0;
    sGamemodeSelectMenu.menu.animFrames = -1;
    sGamemodeSelectMenu.menu.animId = 0;
    sGamemodeSelectMenu.menu.animPhase = 0;
    sGamemodeSelectMenu.selectedDifficulty = 1;
    sGamemodeSelectMenu.selectedChallenge = 0;
    sGamemodeSelectMenu.prevSelection = 0;
}

void handle_inputs_gamemode_select_state_default(f32 stickDir) {
    s32 selection = sGamemodeSelectMenu.menu.selectedMenuIndex;

    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        switch(selection) {
            case GM_SELECT_DIFF:
                menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_CHANGE_DIFF);
                sGamemodeSelectMenu.prevSelection = sGamemodeSelectMenu.selectedDifficulty;
                break;
            case GM_SELECT_CHAL:
                menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_CHANGE_CHALLENGE);
                sGamemodeSelectMenu.prevSelection = sGamemodeSelectMenu.selectedChallenge;
                break;
        }
    } else if(gPlayer1Controller->buttonPressed & D_JPAD || (stickDir == MENU_JOYSTICK_DIR_DOWN)) {
        selection = 1;
    } else if (gPlayer1Controller->buttonPressed & U_JPAD || (stickDir == MENU_JOYSTICK_DIR_UP)) {
        selection = 0;
    }

    sGamemodeSelectMenu.menu.selectedMenuIndex = selection;
}

void handle_inputs_gamemode_select_state_change_diff(f32 stickDir) {
    s32 selection = sGamemodeSelectMenu.selectedDifficulty;
    s32 prevSelection = sGamemodeSelectMenu.prevSelection;

    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
    } else if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
        selection = prevSelection;
    } else if(gPlayer1Controller->buttonPressed & D_JPAD || (stickDir == MENU_JOYSTICK_DIR_DOWN)) {
        selection++;
    } else if (gPlayer1Controller->buttonPressed & U_JPAD || (stickDir == MENU_JOYSTICK_DIR_UP)) {
        selection--;
    }

    if(selection > 2) {
        selection = 0;
    } else if (selection < 0) {
        selection = 2;
    }

    sGamemodeSelectMenu.selectedDifficulty = selection;
}

void handle_inputs_gamemode_select_state_change_challenge(f32 stickDir) {
    s32 selection = sGamemodeSelectMenu.selectedChallenge;
    s32 prevSelection = sGamemodeSelectMenu.prevSelection;

    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
    } else if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
        selection = prevSelection;
    } else if(gPlayer1Controller->buttonPressed & D_JPAD || (stickDir == MENU_JOYSTICK_DIR_DOWN)) {
        selection++;
    } else if (gPlayer1Controller->buttonPressed & U_JPAD || (stickDir == MENU_JOYSTICK_DIR_UP)) {
        selection--;
    }

    if(selection > 1) {
        selection = 0;
    } else if (selection < 0) {
        selection = 1;
    }

    sGamemodeSelectMenu.selectedChallenge = selection;
}

void handle_inputs_gamemode_select_state_confirm(f32 stickDir) {

}

void handle_gamemode_select_inputs() {
    u32 stickDir = menu_update_joystick_dir(&sGamemodeSelectMenu.menu);

    switch(sGamemodeSelectMenu.menu.menuState) {
        case GM_SELECT_STATE_DEFAULT:
            handle_inputs_gamemode_select_state_default(stickDir);
            break;
        case GM_SELECT_STATE_CHANGE_DIFF:
            handle_inputs_gamemode_select_state_change_diff(stickDir);
            break;
        case GM_SELECT_STATE_CHANGE_CHALLENGE:
            handle_inputs_gamemode_select_state_change_challenge(stickDir);
            break;
        case GM_SELECT_STATE_CONFIRM:
            handle_inputs_gamemode_select_state_confirm(stickDir);
            break;
    }
}

void update_gamemode_select() {
    handle_gamemode_select_inputs();
}

/*
    Scrolls the vertices for the ui elements
*/
void scroll_gamemode_select_bg() {
    int i = 0;
	int count = 4;
	int width = 64 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(desc_bg_mesh_mesh_vtx_0);
	Vtx *verticesSquare = segmented_to_virtual(desc_bg_square_square_mesh_mesh_vtx_0);
    Vtx *verticesVertical = segmented_to_virtual(desc_bg_vertical_vertical_mesh_mesh_vtx_0);

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
		verticesSquare[i].n.tc[0] += deltaX;
		verticesSquare[i].n.tc[1] += deltaY;
		verticesVertical[i].n.tc[0] += deltaX;
		verticesVertical[i].n.tc[1] += deltaY;
	}
	currentX += deltaX;	currentY += deltaY;
}

void render_difficulty_select() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx) * 2);
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    s32 cursorY;

    guTranslate(transMtx, DIFF_SELECT_X, DIFF_SELECT_Y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, desc_bg_vertical_vertical_mesh_mesh);

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(-18, 10, "Easy", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(-18, -10, "Normal", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(-18, -30, "Hard", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering();

    cursorY = 15 - (20 * sGamemodeSelectMenu.selectedDifficulty);

    guTranslate(transMtx, -27, cursorY, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    guScale(scaleMtx, 0.75f, 0.75f, 1.0f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_challenge_select() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx) * 2);
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    s32 cursorY;

    guTranslate(transMtx, CHAL_SELECT_X, CHAL_SELECT_Y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, desc_bg_square_square_mesh_mesh);

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(-18, 0, "Classic", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(-18, -20, "Challenge", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering();

    cursorY = 5 - (20 * sGamemodeSelectMenu.selectedChallenge);

    guTranslate(transMtx, -27, cursorY, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    guScale(scaleMtx, 0.75f, 0.75f, 1.0f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_desc_field() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    const char *desc;

    switch(sGamemodeSelectMenu.menu.menuState) {
            case GM_SELECT_STATE_DEFAULT:
            default:
                desc = sGMSelectDescriptions[GM_SELECT_DESC_DIFFICULTY + sGamemodeSelectMenu.menu.selectedMenuIndex];
                break;
            case GM_SELECT_STATE_CHANGE_DIFF:
                desc = sGMSelectDescriptions[GM_SELECT_DESC_EASY + sGamemodeSelectMenu.selectedDifficulty];
                break;
            case GM_SELECT_STATE_CHANGE_CHALLENGE:
                desc = sGMSelectDescriptions[GM_SELECT_DESC_CLASSIC + sGamemodeSelectMenu.selectedChallenge];
                break;
    }

    guTranslate(transMtx, SCREEN_CENTER_X, GM_SELECT_DESC_Y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, desc_bg_mesh_mesh);

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(-142, 15, desc, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_gamemode_select() {
    //Draw cursor for submenu selection
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    s32 cursorY;

    create_dl_ortho_matrix(&gDisplayListHead);

    if(sGamemodeSelectMenu.menu.selectedMenuIndex) {
        //Challenge selection
        cursorY = CHAL_SELECT_Y - 10;
    } else {
        //Difficulty selection
        cursorY = DIFF_SELECT_Y - 10;
    }
    
    guTranslate(transMtx, (CHAL_SELECT_X - 50), cursorY, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    guScale(scaleMtx, 1.5f, 1.5f, 1.0f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    scroll_gamemode_select_bg();
    render_difficulty_select();
    render_challenge_select();
    render_desc_field();
}