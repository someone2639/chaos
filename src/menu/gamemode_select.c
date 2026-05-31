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
#include "game/chaos/chaos.h"

#define DESC_STRING_WIDTH 284

struct GamemodeSelectMenu sGamemodeSelectMenu;

const char *sGMSelectDescriptions[] = {
    [GM_SELECT_DESC_DIFFICULTY]     = "Select game difficulty.",
    [GM_SELECT_DESC_GAMEMODE]       = "Select game mode.",
    [GM_SELECT_DESC_START]          = "Begin the game with the selected settings.",
    [GM_SELECT_DESC_EASY]           = "Patch cards will be more favorable. Cards will feature weaker negative effects.",
    [GM_SELECT_DESC_NORMAL]         = "Patch cards will be well balanced. Cards will have an even distribution of positive and negative effects.",
    [GM_SELECT_DESC_HARD]           = "Patch cards will be more punishing. Cards will feature weaker positive effects, and most negative patches will have a longer duration.",
    [GM_SELECT_DESC_IMPOSSIBLE]     = "Patch cards will be brutally unforgiving. Cards will feature no positive effects whatsoever, and most negative patches will be given relentless durations.",
    [GM_SELECT_DESC_CLASSIC]        = "Infinite lives. Dying will send you out of the course with no additional punishment.",
    [GM_SELECT_DESC_CHALLENGE]      = "Enables lives. Running out of lives will delete the save file. 1-UP mushrooms are removed, and lives may only be increased by collecting yellow stars.",
    [GM_SELECT_DESC_HARDCORE]       = "Only one life. Dying will result in an immediate game over, and permanently delete your save file.",
};

/*
    Sets the gamemode select menu to the state it should be in when the game starts
*/
void init_gamemode_select() {
    sGamemodeSelectMenu.menu.flags = GAMEMODE_SELECT_FLAG_HALT_INPUT;
    sGamemodeSelectMenu.menu.menuState = GM_SELECT_STATE_DEFAULT;
    sGamemodeSelectMenu.menu.index = 0;
    sGamemodeSelectMenu.menu.animTimer = 0;
    sGamemodeSelectMenu.menu.animFrames = -1;
    sGamemodeSelectMenu.menu.animId = 0;
    sGamemodeSelectMenu.menu.animPhase = 0;
    sGamemodeSelectMenu.selectedDifficulty = CHAOS_DIFFICULTY_NORMAL;
    sGamemodeSelectMenu.selectedGameMode = CHAOS_GAMEMODE_CLASSIC;
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
void handle_inputs_gamemode_select_state_default() {
    s32 selection = sGamemodeSelectMenu.menu.index;

    if(gPlayer1Controller->buttonPressed & A_BUTTON) {
        switch(selection) {
            case GM_SELECT_DIFF:
                menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_CHANGE_DIFF);
                sGamemodeSelectMenu.prevSelection = sGamemodeSelectMenu.selectedDifficulty;
                break;
            case GM_SELECT_CHAL:
                menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_CHANGE_GAMEMODE);
                sGamemodeSelectMenu.prevSelection = sGamemodeSelectMenu.selectedGameMode;
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
    } else if (menu_navigate_vertical(&selection, 0, 3, FALSE)) {
        play_sound(SOUND_MENU_MESSAGE_NEXT_PAGE, gGlobalSoundSource);
    }
    
    sGamemodeSelectMenu.menu.index = selection;
}

/*
    Handles inputs for the difficulty selection submenu
*/
void handle_inputs_gamemode_select_state_change_diff() {
    s32 selection = sGamemodeSelectMenu.selectedDifficulty;
    s32 prevSelection = sGamemodeSelectMenu.prevSelection;

    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
    } else if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
        selection = prevSelection;
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
    } else if (menu_navigate_vertical(&selection, 0, CHAOS_DIFFICULTY_COUNT, TRUE)) {
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    }

    sGamemodeSelectMenu.selectedDifficulty = selection;
}

/*
    Handles inputs for the challenge selection menu
*/
void handle_inputs_gamemode_select_state_change_challenge() {
    s32 selection = sGamemodeSelectMenu.selectedGameMode;
    s32 prevSelection = sGamemodeSelectMenu.prevSelection;

    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
    } else if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
        selection = prevSelection;
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
    } else if (menu_navigate_vertical(&selection, 0, CHAOS_GAMEMODE_COUNT, TRUE)) {
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    }

    sGamemodeSelectMenu.selectedGameMode = selection;
}

/*
    Handles inputs for the confirmation dialogue
*/
void handle_inputs_gamemode_select_state_confirm() {
    s32 selection = sGamemodeSelectMenu.menu.index;

    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        if(selection) {
            //No
            menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
            selection = sGamemodeSelectMenu.prevSelection;
            menu_play_anim(&sGamemodeSelectMenu.menu, GM_SELECT_ANIM_SELECTING);
            play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
        } else {
            //Yes
            menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_ENDING);
            play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
        }
    } else if(gPlayer1Controller->buttonPressed & B_BUTTON) {
        menu_set_state(&sGamemodeSelectMenu.menu, GM_SELECT_STATE_DEFAULT);
        selection = sGamemodeSelectMenu.prevSelection;
        menu_play_anim(&sGamemodeSelectMenu.menu, GM_SELECT_ANIM_SELECTING);
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
    } else if (menu_navigate_horizontal(&selection, 0, 2, TRUE)) {
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    }

    sGamemodeSelectMenu.menu.index = selection;
}

/*
    Handles inputs for game select menu
*/
void handle_gamemode_select_inputs() {
    switch(sGamemodeSelectMenu.menu.menuState) {
        case GM_SELECT_STATE_DEFAULT:
            handle_inputs_gamemode_select_state_default();
            break;
        case GM_SELECT_STATE_CHANGE_DIFF:
            handle_inputs_gamemode_select_state_change_diff();
            break;
        case GM_SELECT_STATE_CHANGE_GAMEMODE:
            handle_inputs_gamemode_select_state_change_challenge();
            break;
        case GM_SELECT_STATE_CONFIRM:
            handle_inputs_gamemode_select_state_confirm();
            break;
    }
}

/*
    Plays a startup animation for the gamemode selection menu
*/
#define GM_SELECT_STARTUP_FRAMES       7

s32 gm_select_anim_startup() {
    struct ChaosMenu *menu = &sGamemodeSelectMenu.menu;

    if(menu->animPhase == 0) {
        menu->animFrames = GM_SELECT_STARTUP_FRAMES;
        
        f32 prog = ((f32)menu->animTimer / (f32)menu->animFrames);
        
        //Slide elements on screen
        sGamemodeSelectMenu.startGamePos[0] = menu_anim_f32(prog, MENU_EASE_OUT, GM_START_GAME_X_START, GM_START_GAME_X);
        sGamemodeSelectMenu.chalPos[0] = menu_anim_f32(prog, MENU_EASE_OUT, CHAL_SELECT_X_START, CHAL_SELECT_X);
        sGamemodeSelectMenu.diffPos[0] = menu_anim_f32(prog, MENU_EASE_OUT, DIFF_SELECT_X_START, (DIFF_SELECT_X - 5)); //Offset by 5 so it isn't jarring when the selection animation starts playing
        sGamemodeSelectMenu.descPos[1] = menu_anim_f32(prog, MENU_EASE_OUT, GM_SELECT_DESC_Y_START, GM_SELECT_DESC_Y);

        return FALSE;
    } else {
        menu->flags &= ~GAMEMODE_SELECT_FLAG_HALT_INPUT;
        menu->flags |= GAMEMODE_SELECT_FLAG_DRAW_MAIN_CURSOR;
        return TRUE;
    }
}

/*
    Plays a selection animation
*/
s32 gm_select_anim_selecting() {
    struct ChaosMenu *menu = &sGamemodeSelectMenu.menu;
    s32 selected = menu->index;

    menu->animFrames = MENU_ANIM_LOOP;
    menu->flags |= GAMEMODE_SELECT_FLAG_DRAW_MAIN_CURSOR;

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
    struct ChaosMenu *menu = &sGamemodeSelectMenu.menu;

    menu->animFrames = MENU_ANIM_LOOP;
    menu->flags &= ~GAMEMODE_SELECT_FLAG_DRAW_MAIN_CURSOR;

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
    struct ChaosMenu *menu = &sGamemodeSelectMenu.menu;

    if(menu->animPhase == 0) {
        menu->animFrames = GM_SELECT_RETURN_FRAMES;

        f32 prog = ((f32)menu->animTimer / (f32)menu->animFrames);

        //Slide elements on screen
        sGamemodeSelectMenu.startGamePos[0] = menu_anim_f32(prog, MENU_EASE_IN, GM_START_GAME_X, GM_START_GAME_X_START);
        sGamemodeSelectMenu.chalPos[0] = menu_anim_f32(prog, MENU_EASE_IN, CHAL_SELECT_X, CHAL_SELECT_X_START);
        sGamemodeSelectMenu.diffPos[0] = menu_anim_f32(prog, MENU_EASE_IN, DIFF_SELECT_X, DIFF_SELECT_X_START);
        sGamemodeSelectMenu.descPos[1] = menu_anim_f32(prog, MENU_EASE_IN, GM_SELECT_DESC_Y, GM_SELECT_DESC_Y_START);

        return FALSE;
    } else {
        menu->flags &= ~GAMEMODE_SELECT_FLAG_HALT_INPUT;
        menu->flags |= GAMEMODE_SELECT_FLAG_DRAW_MAIN_CURSOR;
        return TRUE;
    }
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
    Draws the "Start" option on the menu
*/
void render_gm_start_game() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    f32 x = sGamemodeSelectMenu.startGamePos[0];
    f32 y = sGamemodeSelectMenu.startGamePos[1];

    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    Gfx *bg = menu_create_chaos_text_bg(GM_START_GAME_X, GM_START_GAME_Y, 74, 20, 217);
    gSPDisplayList(gDisplayListHead++, bg);

    slowtext_setup_ortho_rendering(&gDisplayListHead, FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(&gDisplayListHead, -5, -10, "Start", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering(&gDisplayListHead);

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
    Gfx *bg = menu_create_chaos_text_bg(DIFF_SELECT_X, DIFF_SELECT_Y, 74, 62, 217);
    gSPDisplayList(gDisplayListHead++, bg);

    slowtext_setup_ortho_rendering(&gDisplayListHead, FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(&gDisplayListHead, -24,  10, "Easy",       FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(&gDisplayListHead, -24,  -3, "Normal",     FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(&gDisplayListHead, -24, -16, "Hard",       FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(&gDisplayListHead, -24, -29, "Impossible", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering(&gDisplayListHead);

    cursorY = 15 - (13 * sGamemodeSelectMenu.selectedDifficulty);

    guTranslate(transMtx, -33, cursorY, 0);
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
    Gfx *bg = menu_create_chaos_text_bg(CHAL_SELECT_X, CHAL_SELECT_Y, 74, 62, 217);
    gSPDisplayList(gDisplayListHead++, bg);

    slowtext_setup_ortho_rendering(&gDisplayListHead, FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(&gDisplayListHead, -24,  7, "Classic",   FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(&gDisplayListHead, -24, -10, "Challenge", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(&gDisplayListHead, -24, -27, "Hardcore",  FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering(&gDisplayListHead);

    cursorY = 12 - (17 * sGamemodeSelectMenu.selectedGameMode);

    guTranslate(transMtx, -33, cursorY, 0);
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
                desc = sGMSelectDescriptions[GM_SELECT_DESC_DIFFICULTY + sGamemodeSelectMenu.menu.index];
                break;
            case GM_SELECT_STATE_CHANGE_DIFF:
                desc = sGMSelectDescriptions[GM_SELECT_DESC_EASY + sGamemodeSelectMenu.selectedDifficulty];
                break;
            case GM_SELECT_STATE_CHANGE_GAMEMODE:
                desc = sGMSelectDescriptions[GM_SELECT_DESC_CLASSIC + sGamemodeSelectMenu.selectedGameMode];
                break;
    }

    slowtext_setup_ortho_rendering(&gDisplayListHead, FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text_linebreaks(&gDisplayListHead, -142, 15, DESC_STRING_WIDTH, desc, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering(&gDisplayListHead);
}

/*
    Writes a confirmation dialogue
*/
void render_gm_confirmation_dialog() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));

    slowtext_setup_ortho_rendering(&gDisplayListHead, FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(&gDisplayListHead, 0, 0, "Begin the game with these settings?", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(&gDisplayListHead, -30, -20, "Yes", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(&gDisplayListHead, 30, -20, "No", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering(&gDisplayListHead);

    s32 selected = sGamemodeSelectMenu.menu.index;
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
    Gfx *bg = menu_create_chaos_text_bg(SCREEN_CENTER_X, GM_SELECT_DESC_Y, 298, 70, 217);
    gSPDisplayList(gDisplayListHead++, bg);

    switch(sGamemodeSelectMenu.menu.menuState) {
        case GM_SELECT_STATE_CONFIRM:
            render_gm_confirmation_dialog();
            break;
        case GM_SELECT_STATE_DEFAULT:
        case GM_SELECT_STATE_CHANGE_DIFF:
        case GM_SELECT_STATE_CHANGE_GAMEMODE:
            render_menu_desc();
            break;
    }

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws the appropriate button prompts for the menu state
*/
void render_gm_select_button_prompts() {
    struct ButtonPromptList prompts = {0};

    switch(sGamemodeSelectMenu.menu.menuState) {
        case GM_SELECT_STATE_DEFAULT:
            menu_add_button_prompt(&prompts, MENU_PROMPT_A_BUTTON, "Select");
            menu_add_button_prompt(&prompts, MENU_PROMPT_START_BUTTON, "Begin");
            menu_add_button_prompt(&prompts, MENU_PROMPT_B_BUTTON, "Back");
            menu_add_button_prompt(&prompts, MENU_PROMPT_Z_TRIG, "Tutorial");
            break;
        case GM_SELECT_STATE_CHANGE_DIFF:
        case GM_SELECT_STATE_CHANGE_GAMEMODE:
        case GM_SELECT_STATE_CONFIRM:
            menu_add_button_prompt(&prompts, MENU_PROMPT_A_BUTTON, "Select");
            menu_add_button_prompt(&prompts, MENU_PROMPT_B_BUTTON, "Back");
            break;
    }

    menu_render_button_prompt_list(&gDisplayListHead, SCREEN_WIDTH - 32, SCREEN_HEIGHT - 33, &prompts);
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

    menu_strip_coverage(&gDisplayListHead);

    create_dl_ortho_matrix(&gDisplayListHead);

    if(flags & GAMEMODE_SELECT_FLAG_DRAW_MAIN_CURSOR) {
        switch(sGamemodeSelectMenu.menu.index) {
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

        guTranslate(transMtx, (CHAL_SELECT_X - 55), cursorY, 0);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
                G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
        guScale(scaleMtx, 0.75f, 0.75f, 1.0f);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
                G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
        gSPDisplayList(gDisplayListHead++, dl_draw_triangle);

        gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    }
    
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