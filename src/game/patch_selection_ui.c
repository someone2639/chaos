#include <ultra64.h>
#include <PR/gbi.h>

#include "patch_selection_ui.h"
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

#define CARD_STRING_WIDTH 94
#define DESC_STRING_WIDTH 284

u8 sQualityColors[CHAOS_PATCH_SEVERITY_COUNT][3] = {
    {0x9F, 0x9F, 0x9F},
    {0x40, 0xFB, 0x3D},
    {0x67, 0x62, 0xFB},
    {0xAB, 0x20, 0xE5},
};

u8 sEffectColors[CHAOS_EFFECT_COUNT][3] = {
    {0x05, 0xDF, 0x15}, //Good
    {0xFF, 0x15, 0x25}, //Bad
};

struct PatchSelectionMenu patchMenu;
struct PatchSelectionMenu *gPatchSelectionMenu = &patchMenu;
/*
    Return number of patches that should be displayed
*/
static s32 get_num_patches() {
    s32 patchCount = DEFAULT_PATCH_DISPLAY_QUANTITY;
    patchCount += chaos_count_active_instances(CHAOS_PATCH_ADD_SELECTABLE_PATCH);
    patchCount -= chaos_count_active_instances(CHAOS_PATCH_REMOVE_SELECTABLE_PATCH);

    if (patchCount > MAX_CARDS) {
        assert_args(FALSE, "get_num_patches:\nPatch display count exceeds maximum!\nActive patch count: %d", patchCount);
        patchCount = MAX_CARDS;
    }
    if (patchCount < 1) {
        assert_args(FALSE, "get_num_patches:\nPatch display count falls below minimum!\nActive patch count: %d", patchCount);
        patchCount = 1;
    }

    return patchCount;
}

/*
    Sets the layout positions and starting positions for the cards in the patch select menu
*/
void init_patch_selection_layout() {
    s32 numPatches = gPatchSelectionMenu->numPatches;

    f32 card1XTarget = CARD_X_LEFT;
    f32 card3XTarget = CARD_X_LEFT;
    f32 topCardYTarget = CARD_Y_TOP;

    //Certain card amounts have different layouts
    switch(numPatches) {
        case 1:
            card1XTarget = CARD_X_MID;
            FALL_THROUGH;
        case 2:
            topCardYTarget = CARD_Y_MID;
            break;
        case 3:
            card3XTarget = CARD_X_MID;
            break;
    }

    //Update layout positions
    gPatchSelectionMenu->patchCards[0].layoutPos[0] = card1XTarget;
    gPatchSelectionMenu->patchCards[0].layoutPos[1] = topCardYTarget;

    gPatchSelectionMenu->patchCards[1].layoutPos[0] = CARD_X_RIGHT;
    gPatchSelectionMenu->patchCards[1].layoutPos[1] = topCardYTarget;

    gPatchSelectionMenu->patchCards[2].layoutPos[0] = card3XTarget;
    gPatchSelectionMenu->patchCards[2].layoutPos[1] = CARD_Y_BOTTOM;

    gPatchSelectionMenu->patchCards[3].layoutPos[0] = CARD_X_RIGHT;
    gPatchSelectionMenu->patchCards[3].layoutPos[1] = CARD_Y_BOTTOM;

    //Set the starting X value of the cards
    gPatchSelectionMenu->patchCards[0].pos[0] = CARD_1_X_START;
    gPatchSelectionMenu->patchCards[1].pos[0] = CARD_2_X_START;
    gPatchSelectionMenu->patchCards[2].pos[0] = CARD_3_X_START;
    gPatchSelectionMenu->patchCards[3].pos[0] = CARD_4_X_START;

    //Set the starting Y value of the cards
    gPatchSelectionMenu->patchCards[0].pos[1] = gPatchSelectionMenu->patchCards[0].layoutPos[1];
    gPatchSelectionMenu->patchCards[1].pos[1] = gPatchSelectionMenu->patchCards[1].layoutPos[1];
    gPatchSelectionMenu->patchCards[2].pos[1] = gPatchSelectionMenu->patchCards[2].layoutPos[1];
    gPatchSelectionMenu->patchCards[3].pos[1] = gPatchSelectionMenu->patchCards[3].layoutPos[1];

    //Set initial scale
    for(int i = 0; i < MAX_CARDS; i++) {
        gPatchSelectionMenu->patchCards[i].scale = CARD_SCALE_DEFAULT;
    }
}

/*
    Loads a fresh batch of patches to select from
*/
void load_new_patches() {
    s32 numPatches = get_num_patches();

    // Generate new patches
    assert(numPatches > 0 && numPatches <= MAX_CARDS, "Tried to load an invalid number of patch cards!");

    struct ChaosPatchSelection *patches = chaos_roll_for_new_patches();
    for (s32 i = 0; i < numPatches; i++) {
        gPatchSelectionMenu->patchCards[i].sel = &patches[i];
    }

    gPatchSelectionMenu->numPatches = numPatches;
    init_patch_selection_layout();
}

/*
    Resets the values of the patch selection menu to their defaults. Should always be called after the patch selection menu is exited.
*/
void reset_patch_selection_menu() {
    // Reset other patch properties
    gPatchSelectionMenu->selectedPatch = 0;
    gPatchSelectionMenu->menu.selectedMenuIndex = 0;
    gPatchSelectionMenu->menu.flags = (PATCH_SELECT_FLAG_HALT_INPUT);
    gPatchSelectionMenu->menu.framesSinceLastStickInput = 0;
    gPatchSelectionMenu->menu.lastStickDir = MENU_JOYSTICK_DIR_NONE;
    gPatchSelectionMenu->menu.menuState = PATCH_SELECT_STATE_SELECT;
    gPatchSelectionMenu->menu.animTimer = 0;
    gPatchSelectionMenu->menu.animFrames = MENU_ANIM_LOOP;
    gPatchSelectionMenu->menu.animId = PATCH_SELECT_ANIM_STARTUP;
    gPatchSelectionMenu->menu.animPhase = 0;
    gPatchSelectionMenu->descPos[0] = PATCH_DESC_X;
    gPatchSelectionMenu->descPos[1] = PATCH_DESC_Y_START;
    gPatchSelectionMenu->curtainPos[0] = SCREEN_CENTER_X;
    gPatchSelectionMenu->curtainPos[1] = CURTAIN_Y_START;
    gPatchSelectionMenu->selectPatchTextPos[0] = SCREEN_CENTER_X;
    gPatchSelectionMenu->selectPatchTextPos[1] = SCREEN_CENTER_Y;
    gPatchSelectionMenu->extendedDescScale = 0.0f;
    gPatchSelectionMenu->selectPatchTextScale = 0.0f;
}

/*
    Handles the player inputs for the patch selection state in the patch selection menu
*/
void handle_inputs_patch_select_state_select(s32 stickDir) {
    s32 previousSelection = gPatchSelectionMenu->selectedPatch;
    s32 selection = previousSelection;
    s32 numPatches = gPatchSelectionMenu->numPatches;
    s32 pressedUpDown = ((gPlayer1Controller->buttonPressed & (D_JPAD | U_JPAD)) || stickDir & (MENU_JOYSTICK_DIR_DOWN | MENU_JOYSTICK_DIR_UP));
    s32 pressedLeftRight = ((gPlayer1Controller->buttonPressed & (L_JPAD | R_JPAD)) || stickDir & (MENU_JOYSTICK_DIR_LEFT | MENU_JOYSTICK_DIR_RIGHT));


    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        menu_play_anim(&gPatchSelectionMenu->menu, PATCH_SELECT_ANIM_CONFIRMATION);
        menu_set_state(&gPatchSelectionMenu->menu, PATCH_SELECT_STATE_CONFIRMATION);
        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
    } else if (gPlayer1Controller->buttonPressed & (Z_TRIG | L_TRIG)) {
        struct PatchCard *selectedCard = &gPatchSelectionMenu->patchCards[gPatchSelectionMenu->selectedPatch];
        aggress(selectedCard, "render_patch_card:\ncard is NULL!");
        aggress(selectedCard->sel, "render_patch_card:\ncard->sel is NULL!");
        struct ChaosPatchSelection *sel = selectedCard->sel;
        const struct ChaosPatch *pos = sel->positivePatch;
        const struct ChaosPatch *neg = sel->negativePatch;

        if(pos->longDescription || neg->longDescription) {
            menu_play_anim(&gPatchSelectionMenu->menu, PATCH_SELECT_ANIM_EXT_DESC);
            menu_set_state(&gPatchSelectionMenu->menu, PATCH_SELECT_STATE_SHOW_EXTENDED_DESC);
            play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
        } else {
            play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
        }
    } else if(pressedUpDown) {
        if(numPatches > 2) {
            selection += 2;
            if(selection > numPatches - 1) {
                selection = previousSelection - 2;
            }
        }
    } else if(pressedLeftRight) {
        selection++;
        if(selection > numPatches - 1 || !(selection % 2)) {
            //Hard coded check to make menu navigation feel more natural with 3 patches
            if(numPatches == 3 && previousSelection == 2) {
                selection = previousSelection;
            } else {
                selection = previousSelection - 1;
            }
        }
    }

    if(selection >= numPatches || selection < 0) {
        selection = numPatches - 1;
    }

    if(selection != previousSelection) {
        play_sound(SOUND_MENU_MESSAGE_NEXT_PAGE, gGlobalSoundSource);
    }

    gPatchSelectionMenu->selectedPatch = selection;
}

/*
    Handles the player inputs for the extended description state in the patch selection menu
*/
void handle_inputs_patch_select_state_show_extended_desc() {
    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON | B_BUTTON | Z_TRIG | L_TRIG)) {
        menu_play_anim(&gPatchSelectionMenu->menu, PATCH_SELECT_ANIM_EXT_DESC_RETURN);
        menu_set_state(&gPatchSelectionMenu->menu, PATCH_SELECT_STATE_SELECT);
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
    }
}

/*
    Handles the player inputs for the confirmation dialog state in the patch selection menu
*/
void handle_inputs_patch_select_state_confirmation(s32 stickDir) {
    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        if(gPatchSelectionMenu->menu.selectedMenuIndex) {
            //No
            menu_play_anim(&gPatchSelectionMenu->menu, PATCH_SELECT_ANIM_CONFIRMATION_RETURN);
            menu_set_state(&gPatchSelectionMenu->menu, PATCH_SELECT_STATE_SELECT);
            play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
            gPatchSelectionMenu->menu.selectedMenuIndex = 0;
        } else {
            //Yes
            menu_play_anim(&gPatchSelectionMenu->menu, PATCH_SELECT_ANIM_ENDING);
            menu_set_state(&gPatchSelectionMenu->menu, PATCH_SELECT_STATE_CLOSING);

            //Play increasingly distressed Mario sounds based on severity of patches
            aggress(gPatchSelectionMenu->patchCards[gPatchSelectionMenu->selectedPatch].sel,
                    "handle_inputs_patch_select_state_confirmation:\nsel undefined!");
            switch(gPatchSelectionMenu->patchCards[gPatchSelectionMenu->selectedPatch].sel->severityLevel) {
                case 1:
                    play_sound(SOUND_MARIO_HERE_WE_GO, gGlobalSoundSource);
                    break;
                case 2:
                    play_sound(SOUND_ARG_LOAD(SOUND_BANK_VOICE,    0x21, 0x00, SOUND_NO_PRIORITY_LOSS | SOUND_DISCRETE), gGlobalSoundSource); //okey dokey sound
                    break;
                case 3:
                    play_sound(SOUND_MARIO_OOOF, gGlobalSoundSource);
                    break;
            }
        }
    } else if(gPlayer1Controller->buttonPressed & B_BUTTON) {
        menu_play_anim(&gPatchSelectionMenu->menu, PATCH_SELECT_ANIM_CONFIRMATION_RETURN);
        menu_set_state(&gPatchSelectionMenu->menu, PATCH_SELECT_STATE_SELECT);
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
        gPatchSelectionMenu->menu.selectedMenuIndex = 0;
    }
    else if(gPlayer1Controller->buttonPressed & R_JPAD || (stickDir & MENU_JOYSTICK_DIR_RIGHT)) {
        gPatchSelectionMenu->menu.selectedMenuIndex++;
        if(gPatchSelectionMenu->menu.selectedMenuIndex > 1) {
            gPatchSelectionMenu->menu.selectedMenuIndex = 0;
        }
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    } else if(gPlayer1Controller->buttonPressed & L_JPAD || (stickDir & MENU_JOYSTICK_DIR_LEFT)) {
        gPatchSelectionMenu->menu.selectedMenuIndex--;
        if(gPatchSelectionMenu->menu.selectedMenuIndex < 0) {
            gPatchSelectionMenu->menu.selectedMenuIndex = 1;
        }
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    }
}

/*
    Handles the player inputs for the patch selection menu
*/
void handle_patch_selection_inputs() {
    u32 stickDir = menu_update_joystick_dir(&gPatchSelectionMenu->menu);

    switch(gPatchSelectionMenu->menu.menuState) {
        case PATCH_SELECT_STATE_SELECT:
            handle_inputs_patch_select_state_select(stickDir);
            break;
        case PATCH_SELECT_STATE_CONFIRMATION:
            handle_inputs_patch_select_state_confirmation(stickDir);
            break;
        case PATCH_SELECT_STATE_SHOW_EXTENDED_DESC:
            handle_inputs_patch_select_state_show_extended_desc();
            break;
    }
}

#define PATCH_SELECT_STARTUP_CURTAIN_DROP_FRAMES        20
#define PATCH_SELECT_STARTUP_CARDS_SLIDE_FRAMES         20
#define PATCH_SELECT_STARTUP_IDLE_FRAMES                9
#define PATCH_SELECT_STARTUP_TEXT_SLIDE_FRAMES          9

/*
    Positions the menu elements to play the startup animation. Returns true when finished.
*/
s32 patch_select_anim_startup() {
    s32 phase = gPatchSelectionMenu->menu.animPhase;
    s32 animTimer = gPatchSelectionMenu->menu.animTimer;
    f32 animPercent;
    f32 card1XTarget = gPatchSelectionMenu->patchCards[0].layoutPos[0];
    f32 card2XTarget = gPatchSelectionMenu->patchCards[1].layoutPos[0];
    f32 card3XTarget = gPatchSelectionMenu->patchCards[2].layoutPos[0];
    f32 card4XTarget = gPatchSelectionMenu->patchCards[3].layoutPos[0];

    switch(phase) {
        case 0:
            if(animTimer == 0) {
                play_sound(SOUND_MENU_CURTAIN_LOWER, gGlobalSoundSource);
            }
            //Drop curtain and scale and show text
            gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_DRAW_START_TEXT;
            gPatchSelectionMenu->menu.animFrames = PATCH_SELECT_STARTUP_CURTAIN_DROP_FRAMES;
            animPercent = sins((0x3FFF / gPatchSelectionMenu->menu.animFrames) * animTimer);
            gPatchSelectionMenu->curtainPos[1] = menu_translate_percentage(CURTAIN_Y_START, CURTAIN_Y_POS, animPercent);
            gPatchSelectionMenu->selectPatchTextScale = approach_f32(gPatchSelectionMenu->selectPatchTextScale, 1.0f, 0.1f, 0.1f);
            break;
        case 1:
            //Cards and description box slide on screen and disable game render
            gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_STOP_GAME_RENDER;
            gPatchSelectionMenu->menu.animFrames = PATCH_SELECT_STARTUP_CARDS_SLIDE_FRAMES;
            animPercent = sins((0x3FFF / gPatchSelectionMenu->menu.animFrames) * animTimer);

            gPatchSelectionMenu->patchCards[0].pos[0] = menu_translate_percentage(gPatchSelectionMenu->patchCards[0].pos[0], card1XTarget, 0.27f);
            gPatchSelectionMenu->patchCards[1].pos[0] = menu_translate_percentage(gPatchSelectionMenu->patchCards[1].pos[0], card2XTarget, 0.27f);
            gPatchSelectionMenu->patchCards[2].pos[0] = menu_translate_percentage(gPatchSelectionMenu->patchCards[2].pos[0], card3XTarget, 0.27f);
            gPatchSelectionMenu->patchCards[3].pos[0] = menu_translate_percentage(gPatchSelectionMenu->patchCards[3].pos[0], card4XTarget, 0.27f);

            gPatchSelectionMenu->descPos[1] = menu_translate_percentage(PATCH_DESC_Y_START, PATCH_DESC_Y, animPercent);
            break;
        case 2:
            //Wait a few frames
            gPatchSelectionMenu->menu.animFrames = PATCH_SELECT_STARTUP_IDLE_FRAMES;
            break;
        case 3:
            //Text goes off screen
            chaosSeqVolSubtractable = TRUE;
            gPatchSelectionMenu->menu.animFrames = PATCH_SELECT_STARTUP_TEXT_SLIDE_FRAMES;
            animPercent = 1.0f - coss((0x3FFF / gPatchSelectionMenu->menu.animFrames) * animTimer);
            gPatchSelectionMenu->selectPatchTextPos[0] = menu_translate_percentage(SCREEN_CENTER_X, SELECT_PATCH_TEXT_END, animPercent);
            break;
        default:
            //End animation, draw the cursor, hide the start text, and move to the next animation
            gPatchSelectionMenu->menu.flags &= ~PATCH_SELECT_FLAG_DRAW_START_TEXT;
            gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_DRAW_CURSOR;
            gPatchSelectionMenu->menu.flags &= ~PATCH_SELECT_FLAG_HALT_INPUT;
            return TRUE;
            break;
    }

    return FALSE;
}

#define PATCH_SELECT_ENDING_CARDS_SLIDE_FRAMES 9
#define PATCH_SELECT_ENDING_CURTAIN_RAISE_FRAMES 15

/*
    Positions the menu elements to play the ending animation. Returns true when finished.
*/
s32 patch_select_anim_ending() {
    s32 phase = gPatchSelectionMenu->menu.animPhase;
    s32 animTimer = gPatchSelectionMenu->menu.animTimer;
    f32 animPercent;

    switch(phase) {
        case 0:
            //Card and description box slides off of the screen and block input
            gPatchSelectionMenu->menu.animFrames = PATCH_SELECT_ENDING_CARDS_SLIDE_FRAMES;
            animPercent = 1.0f - coss((0x3FFF / gPatchSelectionMenu->menu.animFrames) * animTimer);
            gPatchSelectionMenu->descPos[1] = menu_translate_percentage(PATCH_DESC_Y, PATCH_DESC_Y_START, animPercent);
            gPatchSelectionMenu->patchCards[gPatchSelectionMenu->selectedPatch].pos[0] = menu_translate_percentage(PATCH_SELECTED_X, CARD_X_END, animPercent);
            gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_HALT_INPUT;
            break;
        case 1:
            //Curtain raises and turn on game rendering
            if(animTimer == 0) {
                play_sound(SOUND_MENU_CURTAIN_RAISE, gGlobalSoundSource);
            }
            gPatchSelectionMenu->menu.animFrames = PATCH_SELECT_ENDING_CURTAIN_RAISE_FRAMES;
            animPercent = 1.0f - coss((0x3FFF / gPatchSelectionMenu->menu.animFrames) * animTimer);
            gPatchSelectionMenu->menu.flags &= ~PATCH_SELECT_FLAG_STOP_GAME_RENDER;
            gPatchSelectionMenu->curtainPos[1] = menu_translate_percentage(CURTAIN_Y_POS, CURTAIN_Y_START, animPercent);
            break;
        default:
            //End animation and go to the next state
            return TRUE;
            break;
    }

    return FALSE;
}

/*
    Scales the card that the cursor is hovering over
*/

s32 patch_select_anim_select() {
    s32 selected = gPatchSelectionMenu->selectedPatch;

    gPatchSelectionMenu->menu.animFrames = MENU_ANIM_LOOP;
    gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_DRAW_LOWER_TEXT;

    for(int i = 0; i < MAX_CARDS; i++) {
        if(i != selected) {
            gPatchSelectionMenu->patchCards[i].scale = CARD_SCALE_DEFAULT;
        }
    }
    gPatchSelectionMenu->patchCards[selected].scale = CARD_SCALE_HOVER;

    return FALSE;
}

#define PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_ANIM_SCALE_FRAMES 7
#define PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_ANIM_SLIDE_FRAMES 7

/*
    Positions the menu elements to play the confirmation transition animation. Returns true when finished.
*/
s32 patch_select_anim_confirmation() {
    s32 phase = gPatchSelectionMenu->menu.animPhase;
    s32 animTimer = gPatchSelectionMenu->menu.animTimer;
    f32 animPercent;
    f32 xStart = 0, yStart = 0;
    s32 selectedPatch = gPatchSelectionMenu->selectedPatch;
    f32 targetX;

    switch(phase) {
        case 0:
            //If only one card, skip phase
            if(gPatchSelectionMenu->numPatches == 1) {
                gPatchSelectionMenu->menu.animTimer = PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_ANIM_SLIDE_FRAMES;
            }

            //Unselected cards slide off screen and halt input
            gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_HALT_INPUT;
            gPatchSelectionMenu->menu.flags &= ~PATCH_SELECT_FLAG_DRAW_LOWER_TEXT;
            gPatchSelectionMenu->menu.animFrames = PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_ANIM_SLIDE_FRAMES;
            animPercent = 1.0f - coss((0x3FFF / gPatchSelectionMenu->menu.animFrames) * animTimer);
            for(int i = 0; i < MAX_CARDS; i++) {
                if(i != gPatchSelectionMenu->selectedPatch) {
                    targetX = gPatchSelectionMenu->patchCards[i].layoutPos[0];
                    if(i % 2) {
                        gPatchSelectionMenu->patchCards[i].pos[0] = menu_translate_percentage(targetX, CARD_X_RIGHT_START, animPercent);
                    } else {
                        gPatchSelectionMenu->patchCards[i].pos[0] = menu_translate_percentage(targetX, CARD_X_LEFT_START, animPercent);
                    }
                }
            }
            break;
        case 1:
            //Selected card scales up and moves to the center of the screen and hide the cursor
            gPatchSelectionMenu->menu.animFrames = PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_ANIM_SCALE_FRAMES;
            animPercent = sins((0x3FFF / gPatchSelectionMenu->menu.animFrames) * animTimer);

            gPatchSelectionMenu->menu.flags &= ~PATCH_SELECT_FLAG_DRAW_CURSOR;
            gPatchSelectionMenu->patchCards[selectedPatch].scale = menu_translate_percentage(CARD_SCALE_HOVER, CARD_SCALE_SELECTED, animPercent);

            xStart = gPatchSelectionMenu->patchCards[selectedPatch].layoutPos[0];
            yStart = gPatchSelectionMenu->patchCards[selectedPatch].layoutPos[1];
            gPatchSelectionMenu->patchCards[selectedPatch].pos[0] = menu_translate_percentage(xStart, PATCH_SELECTED_X, animPercent);
            gPatchSelectionMenu->patchCards[selectedPatch].pos[1] = menu_translate_percentage(yStart, PATCH_SELECTED_Y, animPercent);
            break;
        default:
            //End animation and turn on input
            gPatchSelectionMenu->menu.flags &= ~PATCH_SELECT_FLAG_HALT_INPUT;
            gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_DRAW_LOWER_TEXT;
            return TRUE;
            break;
    }

    return FALSE;
}

#define PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_RETURN_ANIM_SCALE_FRAMES 7
#define PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_RETURN_ANIM_SLIDE_FRAMES 7

/*
    Positions the menu elements to play the confirmation transition return animation. Returns true when finished.
*/
s32 patch_select_anim_confirmation_return() {
    s32 phase = gPatchSelectionMenu->menu.animPhase;
    s32 animTimer = gPatchSelectionMenu->menu.animTimer;
    f32 animPercent;
    f32 xEnd = 0, yEnd = 0;
    s32 selectedPatch = gPatchSelectionMenu->selectedPatch;
    f32 targetX;

    switch(phase) {
        case 0:
            //Scales the selected card back down and moves it back to its original position
            gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_HALT_INPUT;
            gPatchSelectionMenu->menu.flags &= ~PATCH_SELECT_FLAG_DRAW_LOWER_TEXT;
            gPatchSelectionMenu->menu.animFrames = PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_RETURN_ANIM_SCALE_FRAMES;
            animPercent = sins((0x3FFF / gPatchSelectionMenu->menu.animFrames) * animTimer);

            gPatchSelectionMenu->patchCards[selectedPatch].scale = menu_translate_percentage(CARD_SCALE_SELECTED, CARD_SCALE_HOVER, animPercent);

            xEnd = gPatchSelectionMenu->patchCards[selectedPatch].layoutPos[0];
            yEnd = gPatchSelectionMenu->patchCards[selectedPatch].layoutPos[1];
            gPatchSelectionMenu->patchCards[selectedPatch].pos[0] = menu_translate_percentage(PATCH_SELECTED_X, xEnd, animPercent);
            gPatchSelectionMenu->patchCards[selectedPatch].pos[1] = menu_translate_percentage(PATCH_SELECTED_Y, yEnd, animPercent);
            break;
        case 1:
            //If only one card, skip phase
            if(gPatchSelectionMenu->numPatches == 1) {
                gPatchSelectionMenu->menu.animTimer = PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_RETURN_ANIM_SLIDE_FRAMES;
            }

            //Slides the unselected cards back onto the screen
            gPatchSelectionMenu->menu.animFrames = PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_RETURN_ANIM_SLIDE_FRAMES;
            animPercent = sins((0x3FFF / gPatchSelectionMenu->menu.animFrames) * animTimer);

            for(int i = 0; i < MAX_CARDS; i++) {
                if(i != gPatchSelectionMenu->selectedPatch) {
                    targetX = gPatchSelectionMenu->patchCards[i].layoutPos[0];
                    if(i % 2) {
                        gPatchSelectionMenu->patchCards[i].pos[0] = menu_translate_percentage(CARD_X_RIGHT_START, targetX, animPercent);
                    } else {
                        gPatchSelectionMenu->patchCards[i].pos[0] = menu_translate_percentage(CARD_X_LEFT_START, targetX, animPercent);
                    }
                }
            }
            break;
        default:
            //End the animation, show the cursor, and go to the next state
            gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_DRAW_CURSOR;
            gPatchSelectionMenu->menu.flags &= ~PATCH_SELECT_FLAG_HALT_INPUT;
            return TRUE;
            break;
    }

    return FALSE;
}

#define PATCH_SELECT_MENU_EXT_DESC_APPEAR_ANIM_FRAMES   7

/*
    Updates the menu elements to play the extended decsription appearance animation. Returns true when finished.
*/
s32 patch_select_anim_ext_desc() {
    s32 animTimer = gPatchSelectionMenu->menu.animTimer;
    f32 animPhase = gPatchSelectionMenu->menu.animPhase;

    if(animPhase) {
        gPatchSelectionMenu->menu.flags &= ~PATCH_SELECT_FLAG_HALT_INPUT;
        return TRUE;
    }

    gPatchSelectionMenu->menu.animFrames = PATCH_SELECT_MENU_EXT_DESC_APPEAR_ANIM_FRAMES;
    f32 animPercent = (1.0f / gPatchSelectionMenu->menu.animFrames) * animTimer;

    gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_DRAW_EXTENDED_DESCRIPTION;
    gPatchSelectionMenu->menu.flags &= ~PATCH_SELECT_FLAG_DRAW_CURSOR;
    gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_HALT_INPUT;

    f32 scale = menu_translate_percentage(0.0f, 1.0f, animPercent);
    gPatchSelectionMenu->extendedDescScale = scale;

    return FALSE;
}

#define PATCH_SELECT_MENU_EXT_DESC_DISAPPEAR_ANIM_FRAMES   7

/*
    Updates the menu elements to play the extended decsription appearance animation. Returns true when finished.
*/
s32 patch_select_menu_anim_ext_desc_return() {
    s32 animTimer = gPatchSelectionMenu->menu.animTimer;
    f32 animPhase = gPatchSelectionMenu->menu.animPhase;

    if(animPhase) {
        gPatchSelectionMenu->menu.flags &= ~PATCH_SELECT_FLAG_DRAW_EXTENDED_DESCRIPTION;
        gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_DRAW_CURSOR;
        gPatchSelectionMenu->menu.flags &= ~PATCH_SELECT_FLAG_HALT_INPUT;
        return TRUE;
    }

    gPatchSelectionMenu->menu.animFrames = PATCH_SELECT_MENU_EXT_DESC_DISAPPEAR_ANIM_FRAMES;
    f32 animPercent = (1.0f / gPatchSelectionMenu->menu.animFrames) * animTimer;
    gPatchSelectionMenu->menu.flags |= PATCH_SELECT_FLAG_HALT_INPUT;

    f32 scale = menu_translate_percentage(1.0f, 0.0f, animPercent);
    gPatchSelectionMenu->extendedDescScale = scale;

    return FALSE;
}

s32 (*sPatchSelectMenuAnims[])(void) = {
    &patch_select_anim_startup,
    &patch_select_anim_select,
    &patch_select_anim_confirmation,
    &patch_select_anim_confirmation_return,
    &patch_select_anim_ext_desc,
    &patch_select_menu_anim_ext_desc_return,
    &patch_select_anim_ending,
};

/*
    Updates the patch selection menu
*/
void update_patch_selection_menu() {
    if(!(gPatchSelectionMenu->menu.flags & PATCH_SELECT_FLAG_HALT_INPUT)) {
        handle_patch_selection_inputs();
    }

    if(menu_update_anims(&gPatchSelectionMenu->menu, sPatchSelectMenuAnims)) {
        switch(gPatchSelectionMenu->menu.menuState) {
            case PATCH_SELECT_STATE_SELECT:
                //If animation is finished in the patch select state, we should start the patch select animation
                menu_play_anim(&gPatchSelectionMenu->menu, PATCH_SELECT_ANIM_SELECT);
                break;
            case PATCH_SELECT_STATE_CLOSING:
                //When the closing animation is finished, the menu should close
                menu_set_state(&gPatchSelectionMenu->menu, PATCH_SELECT_STATE_CLOSED);
                chaos_select_patches(gPatchSelectionMenu->patchCards[gPatchSelectionMenu->selectedPatch].sel);
                save_file_do_save(gCurrSaveFileNum - 1);
                break;
        }
    }
}

/*
    Scrolls the vertex uvs for the patch card background meshes
*/
void patch_bg_scroll() {
    int i = 0;
	int count = 4;
	int width = 128 * 0x20;
	int height = 64 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(patch_bg_mesh_mesh_vtx_0);
	Vtx *vertices_r = segmented_to_virtual(patch_bg_r_mesh_r_mesh_vtx_0);

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
		vertices_r[i].n.tc[0] += deltaX;
		vertices_r[i].n.tc[1] += deltaY;
	}
	currentX += deltaX;	currentY += deltaY;
}

/*
    Scrolls the vertex uvs for the descripton and extended description background meshes
*/
void desc_bg_scroll() {
	int i = 0;
	int count = 4;
	int width = 64 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(desc_bg_mesh_mesh_vtx_0);
	Vtx *ext_vertices = segmented_to_virtual(ext_desc_bg_ext_mesh_mesh_vtx_0);

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
		ext_vertices[i].n.tc[0] += deltaX;
		ext_vertices[i].n.tc[1] += deltaY;
	}
	currentX += deltaX;	currentY += deltaY;
}

void render_select_patch_text() {
    f32 scale = gPatchSelectionMenu->selectPatchTextScale;
    f32 posX = gPatchSelectionMenu->selectPatchTextPos[0];
    f32 posY = gPatchSelectionMenu->selectPatchTextPos[1];

    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, posX, posY, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    guScale(scaleMtx, scale, scale, 1.0f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gSPDisplayList(gDisplayListHead++, select_patch_text_mesh_mesh);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_curtain_bg() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, gPatchSelectionMenu->curtainPos[0], gPatchSelectionMenu->curtainPos[1], 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, curtain_Plane_mesh);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws the patch quality beads
*/
void draw_patch_quality(s32 quality) {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx) * 2);
    gSPDisplayList(gDisplayListHead++, patch_quality_bead_begin);
    guTranslate(transMtx, -70, 24, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    for(int i = 0; i < quality; i++) {
        guTranslate(transMtx, 13, 0, 0);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gSPDisplayList(gDisplayListHead++, patch_quality_bead);
    }
    gSPDisplayList(gDisplayListHead++, patch_quality_bead_end);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws a patch type indicator at the given x/y coordinates
*/
void draw_patch_type(f32 x, f32 y, enum ChaosPatchDurationType type) {
    Mtx* transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    if(type == CHAOS_DURATION_STARS || type == CHAOS_DURATION_INFINITE) {
        gSPDisplayList(gDisplayListHead++, star_timer);
    } else if (type == CHAOS_DURATION_USE_COUNT) {
        gSPDisplayList(gDisplayListHead++, uses_counter);
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws a patch card at the given x/y coordinates
*/
void render_patch_card(struct PatchCard *card, s32 reverse) {
    aggress(card, "render_patch_card:\ncard is NULL!");
    aggress(card->sel, "render_patch_card:\ncard->sel is NULL!");

    struct ChaosPatchSelection *sel = card->sel;
    const struct ChaosPatch *pos = sel->positivePatch;
    const struct ChaosPatch *neg = sel->negativePatch;

    s32 quality = sel->severityLevel;
    Mtx *cardScaleMtx = alloc_display_list(sizeof(Mtx));
    Mtx *cardTransMtx = alloc_display_list(sizeof(Mtx));
    char timer1Text[4];
    char timer2Text[4];
    f32 x = card->pos[0];
    f32 y = card->pos[1];
    f32 scale = card->scale;

    gDPSetPrimColor(gDisplayListHead++, 0, 0, 
                    sQualityColors[quality][0], sQualityColors[quality][1], sQualityColors[quality][2], 255);

    //Draw patch bg
    guTranslate(cardTransMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(cardTransMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    if(scale != 1.0f) {
        guScale(cardScaleMtx, scale, scale, 1.0f);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(cardScaleMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    }
    if(reverse) {
        gSPDisplayList(gDisplayListHead++, patch_bg_r_mesh_r_mesh);
    } else {
        gSPDisplayList(gDisplayListHead++, patch_bg_mesh_mesh);
    }
    
    //Draw patch quality beads
    draw_patch_quality(quality);

    //Draw patch type(s)
    gSPDisplayList(gDisplayListHead++, patch_use_type_start);
    if (pos->durationType == CHAOS_DURATION_STARS || pos->durationType == CHAOS_DURATION_USE_COUNT) {
        draw_patch_type(42, 14, pos->durationType);
        assert(pos->duration < 1000, "render_patch_card:\nduration out of range!");
        sprintf(timer1Text, "%d", pos->duration);
    } else if (pos->durationType == CHAOS_DURATION_INFINITE) {
        draw_patch_type(42, 14, pos->durationType);
        sprintf(timer1Text, "`"); // Infinity symbol
    } else {
        timer1Text[0] = '\0';
    }
    if (neg->durationType == CHAOS_DURATION_STARS || neg->durationType == CHAOS_DURATION_USE_COUNT) {
        draw_patch_type(42, -10, neg->durationType);
        assert(neg->duration < 1000, "render_patch_card:\nduration out of range!");
        sprintf(timer2Text, "%d", neg->duration);
    } else if (neg->durationType == CHAOS_DURATION_INFINITE) {
        draw_patch_type(42, -10, neg->durationType);
        sprintf(timer2Text, "`"); // Infinity symbol
    } else {
        timer2Text[0] = '\0';
    }
    gSPDisplayList(gDisplayListHead++, patch_use_type_end);

    //Write text
    slowtext_setup_ortho_rendering(FT_FONT_SMALL_THIN);
    slowtext_draw_ortho_text_linebreaks(-63, 4, CARD_STRING_WIDTH, pos->name, FT_FLAG_ALIGN_LEFT, 
        sEffectColors[EFFECT_COLOR_GOOD][0], sEffectColors[EFFECT_COLOR_GOOD][1], sEffectColors[EFFECT_COLOR_GOOD][2], 0xFF);
    slowtext_draw_ortho_text_linebreaks(-63, -20, CARD_STRING_WIDTH, neg->name, FT_FLAG_ALIGN_LEFT, 
        sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2], 0xFF);
    slowtext_setup_ortho_rendering(FT_FONT_OUTLINE);
    if (timer1Text[0] != '\0') {
        slowtext_draw_ortho_text(51, 4, timer1Text, FT_FLAG_ALIGN_LEFT, 0xD0, 0xC4, 0x00, 0xFF);
    }
    if (timer2Text[0] != '\0') {
        slowtext_draw_ortho_text(51, -20, timer2Text, FT_FLAG_ALIGN_LEFT, 0xD0, 0xC4, 0x00, 0xFF);
    }
    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Writes the patch description
*/
void render_patch_desc() {
    struct ChaosPatchSelection *sel = gPatchSelectionMenu->patchCards[gPatchSelectionMenu->selectedPatch].sel;
    const struct ChaosPatch *pos = sel->positivePatch;
    const struct ChaosPatch *neg = sel->negativePatch;

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text_linebreaks(-142, 15, DESC_STRING_WIDTH, pos->shortDescription, FT_FLAG_ALIGN_LEFT, 
        sEffectColors[EFFECT_COLOR_GOOD][0], sEffectColors[EFFECT_COLOR_GOOD][1], sEffectColors[EFFECT_COLOR_GOOD][2], 0xFF);
    slowtext_draw_ortho_text_linebreaks(-142, -15, DESC_STRING_WIDTH, neg->shortDescription, FT_FLAG_ALIGN_LEFT, 
        sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2], 0xFF);
    slowtext_finished_rendering();
}

/*
    Draws a confirmation dialog
*/
void render_patch_confirmation_dialog() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(0, 0, "Would you like to select this patch?", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(-30, -20, "Yes", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(30, -20, "No", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering();

    s32 selected = gPatchSelectionMenu->menu.selectedMenuIndex;
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
    Draws the patch description at the given x/y coordinates
*/
void render_lower_box(f32 x, f32 y) {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, desc_bg_mesh_mesh);

    if(gPatchSelectionMenu->menu.flags & PATCH_SELECT_FLAG_DRAW_LOWER_TEXT) {
        switch(gPatchSelectionMenu->menu.menuState) {
            case PATCH_SELECT_STATE_CONFIRMATION:
                render_patch_confirmation_dialog();
                break;
            case PATCH_SELECT_STATE_SELECT:
                render_patch_desc();
                break;
        }
    }

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws the cursor for the patch selection menu at the given x/y coordinates
*/
void render_patch_selection_cursor(f32 x, f32 y) {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, x + 65, y - 30, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    
    gSPDisplayList(gDisplayListHead++, hand_cursor_start);
    if(gPatchSelectionMenu->menu.animId == PATCH_SELECT_ANIM_CONFIRMATION) {
        gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_8b_LOAD_BLOCK, 1, hand_closed_hand_closed_ia8);
    } else {
        gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_8b_LOAD_BLOCK, 1, hand_hand_open_ia8);
    }
    gSPDisplayList(gDisplayListHead++, hand_cursor_end);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Shows a dialogue box featuring the extended descriptions of the currently selected card
*/
void render_extended_description() {
    s32 selected = gPatchSelectionMenu->selectedPatch;
    struct PatchCard *selectedCard = &gPatchSelectionMenu->patchCards[selected];
    f32 scale = gPatchSelectionMenu->extendedDescScale;

    struct ChaosPatchSelection *sel = selectedCard->sel;
    const struct ChaosPatch *pos = sel->positivePatch;
    const struct ChaosPatch *neg = sel->negativePatch;
    
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, SCREEN_CENTER_X, SCREEN_CENTER_Y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    guScale(scaleMtx, 1.0f, scale, 1.0f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gSPDisplayList(gDisplayListHead++, ext_desc_bg_ext_mesh_mesh);

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    if(pos->longDescription) {
        slowtext_draw_ortho_text_linebreaks(-142, 87, DESC_STRING_WIDTH, pos->longDescription, FT_FLAG_ALIGN_LEFT, 
            sEffectColors[EFFECT_COLOR_GOOD][0], sEffectColors[EFFECT_COLOR_GOOD][1], sEffectColors[EFFECT_COLOR_GOOD][2], 0xFF);
    }
    if(neg->longDescription) {
        //Draw second effect description lower if there are two extended descriptions
        if(pos->longDescription) {
            slowtext_draw_ortho_text_linebreaks(-142, -13, DESC_STRING_WIDTH, neg->longDescription, FT_FLAG_ALIGN_LEFT, 
                sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2], 0xFF);
        } else {
            slowtext_draw_ortho_text_linebreaks(-142, 87, DESC_STRING_WIDTH, neg->longDescription, FT_FLAG_ALIGN_LEFT, 
                sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2], 0xFF);
        }
    }
    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Determines which button prompts if any to show and draws them
*/
void render_patch_select_button_prompts() {
    struct ChaosPatchSelection *sel = gPatchSelectionMenu->patchCards[gPatchSelectionMenu->selectedPatch].sel;
    const struct ChaosPatch *pos = sel->positivePatch;
    const struct ChaosPatch *neg = sel->negativePatch;

    switch(gPatchSelectionMenu->menu.menuState) {
        case PATCH_SELECT_STATE_SELECT:
            if(pos->longDescription || 
                neg->longDescription) {
                menu_start_button_prompt();
                menu_button_prompt(SCREEN_WIDTH - 32, SCREEN_HEIGHT - 21, MENU_PROMPT_A_BUTTON);
                menu_button_prompt(SCREEN_WIDTH - 82, SCREEN_HEIGHT - 21, MENU_PROMPT_Z_TRIG);
                menu_end_button_prompt();
                fasttext_setup_textrect_rendering(FT_FONT_SMALL_THIN);
                fasttext_draw_texrect(SCREEN_WIDTH - 33, SCREEN_HEIGHT - 21, "Select", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
                fasttext_draw_texrect(SCREEN_WIDTH - 83, SCREEN_HEIGHT - 21, "Info", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
                fasttext_finished_rendering();
            } else {
                menu_start_button_prompt();
                menu_button_prompt(SCREEN_WIDTH - 32, SCREEN_HEIGHT - 21, MENU_PROMPT_A_BUTTON);
                menu_end_button_prompt();
                fasttext_setup_textrect_rendering(FT_FONT_SMALL_THIN);
                fasttext_draw_texrect(SCREEN_WIDTH - 33, SCREEN_HEIGHT - 21, "Select", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
                fasttext_finished_rendering();
            }
            break;
        case PATCH_SELECT_STATE_CONFIRMATION:
            menu_start_button_prompt();
            menu_button_prompt(SCREEN_WIDTH - 32, SCREEN_HEIGHT - 21, MENU_PROMPT_A_BUTTON);
            menu_button_prompt(SCREEN_WIDTH - 82, SCREEN_HEIGHT - 21, MENU_PROMPT_B_BUTTON);
            menu_end_button_prompt();
            fasttext_setup_textrect_rendering(FT_FONT_SMALL_THIN);
            fasttext_draw_texrect(SCREEN_WIDTH - 33, SCREEN_HEIGHT - 21, "Select", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
            fasttext_draw_texrect(SCREEN_WIDTH - 83, SCREEN_HEIGHT - 21, "Back", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
            fasttext_finished_rendering();
            break;
        case PATCH_SELECT_STATE_SHOW_EXTENDED_DESC:
            menu_start_button_prompt();
            menu_button_prompt(SCREEN_WIDTH - 32, SCREEN_HEIGHT - 30, MENU_PROMPT_B_BUTTON);
            menu_end_button_prompt();
            fasttext_setup_textrect_rendering(FT_FONT_SMALL_THIN);
            fasttext_draw_texrect(SCREEN_WIDTH - 33, SCREEN_HEIGHT - 30, "Back", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
            fasttext_finished_rendering();
            break;
    }
}

/*
    Displays the patch selection menu
*/
void display_patch_selection_ui() {
    s32 selectedPatch = gPatchSelectionMenu->selectedPatch;
    s32 numPatches = gPatchSelectionMenu->numPatches;
    f32 cursorX, cursorY;

    cursorX = gPatchSelectionMenu->patchCards[selectedPatch].layoutPos[0];
    cursorY = gPatchSelectionMenu->patchCards[selectedPatch].layoutPos[1];

    if(gPatchSelectionMenu->menu.menuState != PATCH_SELECT_STATE_CLOSED) {
        patch_bg_scroll();
        desc_bg_scroll();
        create_dl_ortho_matrix(&gDisplayListHead);

        render_curtain_bg();
        switch(numPatches) {
            case 4:
                render_patch_card(&gPatchSelectionMenu->patchCards[3], TRUE);
                FALL_THROUGH;
            case 3:
                render_patch_card(&gPatchSelectionMenu->patchCards[2], FALSE);
                FALL_THROUGH;
            case 2:
                render_patch_card(&gPatchSelectionMenu->patchCards[1], TRUE);
                FALL_THROUGH;
            default:
                render_patch_card(&gPatchSelectionMenu->patchCards[0], FALSE);
                break;
        }

        render_lower_box(gPatchSelectionMenu->descPos[0], gPatchSelectionMenu->descPos[1]);

        if(gPatchSelectionMenu->menu.flags & PATCH_SELECT_FLAG_DRAW_START_TEXT) {
            render_select_patch_text();
        }

        if(gPatchSelectionMenu->menu.flags & PATCH_SELECT_FLAG_DRAW_EXTENDED_DESCRIPTION) {
            render_extended_description();
        }

        if(gPatchSelectionMenu->menu.flags & PATCH_SELECT_FLAG_DRAW_CURSOR) {
            render_patch_selection_cursor(cursorX, cursorY);
        }

        if(!(gPatchSelectionMenu->menu.flags & PATCH_SELECT_FLAG_HALT_INPUT)) {
            render_patch_select_button_prompts();
        }
    }
}