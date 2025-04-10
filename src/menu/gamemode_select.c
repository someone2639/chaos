#include <ultra64.h>
#include <PR/gbi.h>

#include "game/object_helpers.h"
#include "gamemode_select.h"
#include "game/segment2.h"
#include "src/engine/math_util.h"
#include "game/game_init.h"
#include "game/fasttext.h"
#include "game/ingame_menu.h"
#include "audio/external.h"
#include "sm64.h"
#include "game/chaos_tutorial.h"

#define DESC_STRING_WIDTH 284

struct GamemodeSelectMenu sGamemodeSelectMenu;

const char *sGMSelectDescriptions[] = {
    [GM_SELECT_DESC_DIFFICULTY]     = "Select game difficulty.",
    [GM_SELECT_DESC_GAMEMODE]       = "Select game mode.",
    [GM_SELECT_DESC_START]          = "Begin the game with the selected settings.",
    [GM_SELECT_DESC_EASY]           = "Patch cards will be more favorable. Cards will feature weaker negative effects.",
    [GM_SELECT_DESC_NORMAL]         = "Patch cards will be evenly balanced. Cards will have equal positive and negative effects.",
    [GM_SELECT_DESC_HARD]           = "Patch cards will be more punishing. Cards will feature weaker positive effects and most negative patches will have a longer duration.",
    [GM_SELECT_DESC_CLASSIC]        = "Infinite Lives. Dying will send you out of the course with no additional punishment.",
    [GM_SELECT_DESC_CHALLENGE]      = "Enables Lives. Running out of lives will delete the save file. 1-UP mushrooms are removed, and lives may only be increased by collecting yellow stars."
};

/*
    Sets the gamemode select menu to the state it should be in when the game starts
*/
void init_gamemode_select() {
    sGamemodeSelectMenu.menu.flags = GAMEMODE_SELECT_FLAG_HALT_INPUT;
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

    sGamemodeSelectMenu.diffPos[0] = DIFF_SELECT_X_START;
    sGamemodeSelectMenu.diffPos[1] = DIFF_SELECT_Y;
    sGamemodeSelectMenu.chalPos[0] = CHAL_SELECT_X_START;
    sGamemodeSelectMenu.chalPos[1] = CHAL_SELECT_Y;
    sGamemodeSelectMenu.startGamePos[0] = GM_START_GAME_X_START;
    sGamemodeSelectMenu.startGamePos[1] = GM_START_GAME_Y;
    sGamemodeSelectMenu.descPos[0] = SCREEN_CENTER_X;
    sGamemodeSelectMenu.descPos[1] = GM_SELECT_DESC_Y_START;
}

/*
    Handles inputs for the main selection menu
*/
void handle_inputs_gamemode_select_state_default(u32 stickDir) {
    s32 selection = sGamemodeSelectMenu.menu.selectedMenuIndex;
    s32 prevSelection = selection;
    s32 playCursorSound = FALSE;

    if(gPlayer1Controller->buttonPressed & A_BUTTON) {
        switch(selection) {
            case GM_SELECT_DIFF:
                menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_CHANGE_DIFF);
                sGamemodeSelectMenu.prevSelection = sGamemodeSelectMenu.selectedDifficulty;
                break;
            case GM_SELECT_CHAL:
                menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_CHANGE_CHALLENGE);
                sGamemodeSelectMenu.prevSelection = sGamemodeSelectMenu.selectedChallenge;
                break;
            case GM_SELECT_START:
                sGamemodeSelectMenu.prevSelection = selection;
                menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_CONFIRM);
                selection = 0;
                menu_play_anim(&sGamemodeSelectMenu.menu, GM_SELECT_ANIM_CONFIRM);
                break;
        }
        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
    } else if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        sGamemodeSelectMenu.prevSelection = selection;
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_CONFIRM);
        selection = 0;
        menu_play_anim(&sGamemodeSelectMenu.menu, GM_SELECT_ANIM_CONFIRM);
        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
    } else if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_RETURN);
        menu_play_anim(&sGamemodeSelectMenu.menu, GM_SELECT_ANIM_RETURN);
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
    } else if (gPlayer1Controller->buttonPressed & (Z_TRIG | L_TRIG)) {
        chstut_tutorial_init();
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_TUTORIAL);
        sGamemodeSelectMenu.menu.flags |= GAMEMODE_SELECT_FLAG_HALT_INPUT;
    } else if(gPlayer1Controller->buttonPressed & D_JPAD || (stickDir == MENU_JOYSTICK_DIR_DOWN)) {
        selection++;
        playCursorSound = TRUE;
    } else if (gPlayer1Controller->buttonPressed & U_JPAD || (stickDir == MENU_JOYSTICK_DIR_UP)) {
        selection--;
        playCursorSound = TRUE;
    }

    if(selection < 0 || selection > 2) {
        selection = prevSelection;
        playCursorSound = FALSE;
    }

    if(playCursorSound) {
        play_sound(SOUND_MENU_MESSAGE_NEXT_PAGE, gGlobalSoundSource);
    }

    sGamemodeSelectMenu.menu.selectedMenuIndex = selection;
}

/*
    Handles inputs for the difficulty selection submenu
*/
void handle_inputs_gamemode_select_state_change_diff(u32 stickDir) {
    s32 selection = sGamemodeSelectMenu.selectedDifficulty;
    s32 prevSelection = sGamemodeSelectMenu.prevSelection;

    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
    } else if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
        selection = prevSelection;
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
    } else if(gPlayer1Controller->buttonPressed & D_JPAD || (stickDir == MENU_JOYSTICK_DIR_DOWN)) {
        selection++;
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    } else if (gPlayer1Controller->buttonPressed & U_JPAD || (stickDir == MENU_JOYSTICK_DIR_UP)) {
        selection--;
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    }

    if(selection > 2) {
        selection = 0;
    } else if (selection < 0) {
        selection = 2;
    }

    sGamemodeSelectMenu.selectedDifficulty = selection;
}

/*
    Handles inputs for the challenge selection menu
*/
void handle_inputs_gamemode_select_state_change_challenge(u32 stickDir) {
    s32 selection = sGamemodeSelectMenu.selectedChallenge;
    s32 prevSelection = sGamemodeSelectMenu.prevSelection;

    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
    } else if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
        selection = prevSelection;
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
    } else if(gPlayer1Controller->buttonPressed & D_JPAD || (stickDir == MENU_JOYSTICK_DIR_DOWN)) {
        selection++;
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    } else if (gPlayer1Controller->buttonPressed & U_JPAD || (stickDir == MENU_JOYSTICK_DIR_UP)) {
        selection--;
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    }

    if(selection > 1) {
        selection = 0;
    } else if (selection < 0) {
        selection = 1;
    }

    sGamemodeSelectMenu.selectedChallenge = selection;
}

/*
    Handles inputs for the confirmation dialogue
*/
void handle_inputs_gamemode_select_state_confirm(u32 stickDir) {
    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        if(sGamemodeSelectMenu.menu.selectedMenuIndex) {
            //No
            menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
            sGamemodeSelectMenu.menu.selectedMenuIndex = sGamemodeSelectMenu.prevSelection;
            menu_play_anim(&sGamemodeSelectMenu.menu, GM_SELECT_ANIM_SELECTING);
            play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
        } else {
            //Yes
            menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_ENDING);
            play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
        }
    } else if(gPlayer1Controller->buttonPressed & B_BUTTON) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
        sGamemodeSelectMenu.menu.selectedMenuIndex = sGamemodeSelectMenu.prevSelection;
        menu_play_anim(&sGamemodeSelectMenu.menu, GM_SELECT_ANIM_SELECTING);
    }
    else if(gPlayer1Controller->buttonPressed & R_JPAD || (stickDir & MENU_JOYSTICK_DIR_RIGHT)) {
        sGamemodeSelectMenu.menu.selectedMenuIndex++;
        if(sGamemodeSelectMenu.menu.selectedMenuIndex > 1) {
            sGamemodeSelectMenu.menu.selectedMenuIndex = 0;
        }
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    } else if(gPlayer1Controller->buttonPressed & L_JPAD || (stickDir & MENU_JOYSTICK_DIR_LEFT)) {
        sGamemodeSelectMenu.menu.selectedMenuIndex--;
        if(sGamemodeSelectMenu.menu.selectedMenuIndex < 0) {
            sGamemodeSelectMenu.menu.selectedMenuIndex = 1;
        }
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    }
}

/*
    Handles inputs for game select menu
*/
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

/*
    Plays a startup animation for the gamemode selection menu
*/
#define GM_SELECT_STARTUP_FRAMES       7

s32 gm_select_anim_startup() {
    s32 phase = sGamemodeSelectMenu.menu.animPhase;
    s32 animTimer = sGamemodeSelectMenu.menu.animTimer;

    if(phase) {
        sGamemodeSelectMenu.menu.flags &= ~GAMEMODE_SELECT_FLAG_HALT_INPUT;
        sGamemodeSelectMenu.menu.flags |= GAMEMODE_SELECT_FLAG_DRAW_MAIN_CURSOR;
        return TRUE;
    }
    
    sGamemodeSelectMenu.menu.animFrames = GM_SELECT_STARTUP_FRAMES;
    f32 animPercent = sins((0x3FFF / sGamemodeSelectMenu.menu.animFrames) * animTimer);

    //Slide elements on screen
    f32 startGameX = menu_translate_percentage(GM_START_GAME_X_START, GM_START_GAME_X, animPercent);
    f32 challX = menu_translate_percentage(CHAL_SELECT_X_START, CHAL_SELECT_X, animPercent);
    f32 diffX = menu_translate_percentage(DIFF_SELECT_X_START, (DIFF_SELECT_X - 5), animPercent); //Offset by 5 so it isn't jarring when the selection animation starts playing
    f32 descY = menu_translate_percentage(GM_SELECT_DESC_Y_START, GM_SELECT_DESC_Y, animPercent);

    sGamemodeSelectMenu.diffPos[0] = diffX;
    sGamemodeSelectMenu.chalPos[0] = challX;
    sGamemodeSelectMenu.startGamePos[0] = startGameX;
    sGamemodeSelectMenu.descPos[1] = descY;
    
    return FALSE;
}

/*
    Plays a selection animation
*/
s32 gm_select_anim_selecting() {
    s32 selected = sGamemodeSelectMenu.menu.selectedMenuIndex;

    sGamemodeSelectMenu.menu.animFrames = MENU_ANIM_LOOP;
    sGamemodeSelectMenu.menu.flags |= GAMEMODE_SELECT_FLAG_DRAW_MAIN_CURSOR;

    sGamemodeSelectMenu.diffPos[0] = DIFF_SELECT_X;
    sGamemodeSelectMenu.chalPos[0] = CHAL_SELECT_X;
    sGamemodeSelectMenu.startGamePos[0] = GM_START_GAME_X;

    //Shift selected element to the side
    switch(selected) {
        case GM_SELECT_DIFF:
            sGamemodeSelectMenu.diffPos[0] -= 5;
            break;
        case GM_SELECT_CHAL:
            sGamemodeSelectMenu.chalPos[0] -= 5;
            break;
        case GM_SELECT_START:
            sGamemodeSelectMenu.startGamePos[0] -= 5;
            break;
    }

    return FALSE;
}

/*
    Plays the confirmation animation
*/
s32 gm_select_anim_confirmation() {
    sGamemodeSelectMenu.menu.animFrames = MENU_ANIM_LOOP;
    sGamemodeSelectMenu.menu.flags &= ~GAMEMODE_SELECT_FLAG_DRAW_MAIN_CURSOR;

    sGamemodeSelectMenu.diffPos[0] = DIFF_SELECT_X;
    sGamemodeSelectMenu.chalPos[0] = CHAL_SELECT_X;
    sGamemodeSelectMenu.startGamePos[0] = GM_START_GAME_X;

    return FALSE;
}

/*
    Plays a return animation for the gamemode selection menu
*/
#define GM_SELECT_RETURN_FRAMES       7

s32 gm_select_anim_return() {
    s32 phase = sGamemodeSelectMenu.menu.animPhase;
    s32 animTimer = sGamemodeSelectMenu.menu.animTimer;

    if(phase) {
        sGamemodeSelectMenu.menu.flags &= ~GAMEMODE_SELECT_FLAG_HALT_INPUT;
        sGamemodeSelectMenu.menu.flags |= GAMEMODE_SELECT_FLAG_DRAW_MAIN_CURSOR;
        return TRUE;
    }
    
    sGamemodeSelectMenu.menu.animFrames = GM_SELECT_RETURN_FRAMES;
    f32 animPercent = sins((0x3FFF / sGamemodeSelectMenu.menu.animFrames) * animTimer);

    //Slide elements on screen
    f32 startGameX = menu_translate_percentage(GM_START_GAME_X, GM_START_GAME_X_START, animPercent);
    f32 challX = menu_translate_percentage(CHAL_SELECT_X, CHAL_SELECT_X_START, animPercent);
    f32 diffX = menu_translate_percentage(DIFF_SELECT_X, DIFF_SELECT_X_START, animPercent);
    f32 descY = menu_translate_percentage(GM_SELECT_DESC_Y, GM_SELECT_DESC_Y_START, animPercent);

    sGamemodeSelectMenu.diffPos[0] = diffX;
    sGamemodeSelectMenu.chalPos[0] = challX;
    sGamemodeSelectMenu.startGamePos[0] = startGameX;
    sGamemodeSelectMenu.descPos[1] = descY;
    
    return FALSE;
}

s32 (*sGMSelectAnims[])(void) = {
    &gm_select_anim_startup,
    &gm_select_anim_selecting,
    &gm_select_anim_confirmation,
    &gm_select_anim_return,
};

/*
    Main update loop for gamemode selection
*/
s32 update_gamemode_select() {
    if(!(sGamemodeSelectMenu.menu.flags & GAMEMODE_SELECT_FLAG_HALT_INPUT)) {
        handle_gamemode_select_inputs();
    } else if (sGamemodeSelectMenu.menu.menuState == GM_SELECT_STATE_TUTORIAL) {
        if(chstut_update_tutorial()) {
            menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
            sGamemodeSelectMenu.menu.flags &= ~GAMEMODE_SELECT_FLAG_HALT_INPUT;
        }
    }

    if(menu_update_anims(&sGamemodeSelectMenu.menu, sGMSelectAnims)) {
        if(sGamemodeSelectMenu.menu.menuState == GM_SELECT_STATE_RETURN) {
            init_gamemode_select(); //Also doubles as a reset function!!
            return -1;
        } else {
            menu_play_anim(&sGamemodeSelectMenu.menu, GM_SELECT_ANIM_SELECTING);
        }
    }

    if(sGamemodeSelectMenu.menu.menuState == GM_SELECT_STATE_ENDING) {
        return 1;
    }
    
    return 0;
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
	Vtx *verticesDiff = segmented_to_virtual(desc_bg_diff_diff_mesh_mesh_vtx_0);
	Vtx *verticesChal = segmented_to_virtual(desc_bg_chal_chal_mesh_mesh_vtx_0);
    Vtx *verticesStart = segmented_to_virtual(desc_bg_start_start_mesh_mesh_vtx_0);

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
		verticesDiff[i].n.tc[0] += deltaX;
		verticesDiff[i].n.tc[1] += deltaY;
		verticesChal[i].n.tc[0] += deltaX;
		verticesChal[i].n.tc[1] += deltaY;
		verticesStart[i].n.tc[0] += deltaX;
		verticesStart[i].n.tc[1] += deltaY;
	}
	currentX += deltaX;	currentY += deltaY;
}

/*
    Draws the "Start" option on the menu
*/
void render_gm_start_game() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    f32 x = sGamemodeSelectMenu.startGamePos[0];
    f32 y = sGamemodeSelectMenu.startGamePos[1];

    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, desc_bg_start_start_mesh_mesh);

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(0, -10, "Start", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws the difficulty selection options on the menu
*/
void render_difficulty_select() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx) * 2);
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    s32 cursorY;
    f32 x = sGamemodeSelectMenu.diffPos[0];
    f32 y = sGamemodeSelectMenu.diffPos[1];

    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, desc_bg_diff_diff_mesh_mesh);

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

/*
    Draws the challenge selection options on the menu
*/
void render_challenge_select() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx) * 2);
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    s32 cursorY;
    f32 x = sGamemodeSelectMenu.chalPos[0];
    f32 y = sGamemodeSelectMenu.chalPos[1];

    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, desc_bg_chal_chal_mesh_mesh);

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

/*
    Writes a description based on the currently highlighted menu option
*/
void render_menu_desc() {
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

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text_linebreaks(-142, 15, DESC_STRING_WIDTH, desc, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering();
}

/*
    Writes a confirmation dialogue
*/
void render_gm_confirmation_dialog() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(0, 0, "Begin the game with these settings?", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(-30, -20, "Yes", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(30, -20, "No", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering();

    s32 selected = sGamemodeSelectMenu.menu.selectedMenuIndex;
    f32 xPos;

    if(selected) {
        xPos = 10;
    } else {
        xPos = -50;
    }

    guTranslate(transMtx, xPos, -15, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    guScale(scaleMtx, 0.75f, 0.75f, 1.0f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws the box that contains either the description field or the confirmation dialogue on the menu
*/
void render_desc_field() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    f32 x = sGamemodeSelectMenu.descPos[0];
    f32 y = sGamemodeSelectMenu.descPos[1];
    

    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, desc_bg_mesh_mesh);

    switch(sGamemodeSelectMenu.menu.menuState) {
        case GM_SELECT_STATE_CONFIRM:
            render_gm_confirmation_dialog();
            break;
        case GM_SELECT_STATE_DEFAULT:
        case GM_SELECT_STATE_CHANGE_DIFF:
        case GM_SELECT_STATE_CHANGE_CHALLENGE:
            render_menu_desc();
            break;
    }

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws the appropriate button prompts for the menu state
*/
void render_gm_select_button_prompts() {
    switch(sGamemodeSelectMenu.menu.menuState) {
        case GM_SELECT_STATE_DEFAULT:
            menu_start_button_prompt();
            menu_button_prompt(SCREEN_WIDTH - 32, SCREEN_HEIGHT - 33, MENU_PROMPT_A_BUTTON);
            menu_button_prompt(SCREEN_WIDTH - 77, SCREEN_HEIGHT - 33, MENU_PROMPT_START_BUTTON);
            menu_button_prompt(SCREEN_WIDTH - 119, SCREEN_HEIGHT - 33, MENU_PROMPT_B_BUTTON);
            menu_button_prompt(SCREEN_WIDTH - 157, SCREEN_HEIGHT - 33, MENU_PROMPT_Z_TRIG);
            menu_end_button_prompt();
            fasttext_setup_textrect_rendering(FT_FONT_SMALL_THIN);
            fasttext_draw_texrect(SCREEN_WIDTH - 33, SCREEN_HEIGHT - 33, "Select", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
            fasttext_draw_texrect(SCREEN_WIDTH - 78, SCREEN_HEIGHT - 33, "Begin", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
            fasttext_draw_texrect(SCREEN_WIDTH - 120, SCREEN_HEIGHT - 33, "Back", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
            fasttext_draw_texrect(SCREEN_WIDTH - 158, SCREEN_HEIGHT - 33, "Tutorial", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
            fasttext_finished_rendering();
            break;
        case GM_SELECT_STATE_CHANGE_DIFF:
        case GM_SELECT_STATE_CHANGE_CHALLENGE:
        case GM_SELECT_STATE_CONFIRM:
            menu_start_button_prompt();
            menu_button_prompt(SCREEN_WIDTH - 32, SCREEN_HEIGHT - 33, MENU_PROMPT_A_BUTTON);
            menu_button_prompt(SCREEN_WIDTH - 82, SCREEN_HEIGHT - 33, MENU_PROMPT_B_BUTTON);
            menu_end_button_prompt();
            fasttext_setup_textrect_rendering(FT_FONT_SMALL_THIN);
            fasttext_draw_texrect(SCREEN_WIDTH - 33, SCREEN_HEIGHT - 33, "Select", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
            fasttext_draw_texrect(SCREEN_WIDTH - 83, SCREEN_HEIGHT - 33, "Back", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
            fasttext_finished_rendering();
            break;
    }
}

/*
    Draws the gamemode selection menu
*/
void render_gamemode_select() {
    //Draw cursor for submenu selection
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    s32 cursorY;
    u32 flags = sGamemodeSelectMenu.menu.flags;

    // Coverage meme
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineLERP(gDisplayListHead++, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT);
    gDPSetFillColor(gDisplayListHead++, (GPACK_RGBA5551(0, 0, 0, 1) << 16) | GPACK_RGBA5551(0, 0, 0, 1));
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 0);
    gDPFillRectangle(gDisplayListHead++, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);

    create_dl_ortho_matrix(&gDisplayListHead);

    if(flags & GAMEMODE_SELECT_FLAG_DRAW_MAIN_CURSOR) {
        switch(sGamemodeSelectMenu.menu.selectedMenuIndex) {
            case GM_SELECT_DIFF:
                //Difficulty selection
                cursorY = DIFF_SELECT_Y - 5;
                break;
            case GM_SELECT_CHAL:
                //Challenge selection
                cursorY = CHAL_SELECT_Y - 5;
                break;
            case GM_SELECT_START:
            default:
                //Begin Game
                cursorY = GM_START_GAME_Y - 5;
                break;
        }

        guTranslate(transMtx, (CHAL_SELECT_X - 45), cursorY, 0);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
                G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
        guScale(scaleMtx, 0.75f, 0.75f, 1.0f);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
                G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
        gSPDisplayList(gDisplayListHead++, dl_draw_triangle);

        gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    }
    
    scroll_gamemode_select_bg();
    render_difficulty_select();
    render_challenge_select();
    render_gm_start_game();
    render_desc_field();

    if(!(flags & GAMEMODE_SELECT_FLAG_HALT_INPUT)) {
        render_gm_select_button_prompts();
    }

    if(sGamemodeSelectMenu.menu.menuState == GM_SELECT_STATE_TUTORIAL) {
        chstut_render_tutorial();
    }
}