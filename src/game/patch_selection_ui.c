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
#include "debug.h"
#include "src/engine/math_util.h"

u8 sQualityColors[][3] = {
    {0x20, 0xDB, 0x1D},
    {0x47, 0x42, 0xDB},
    {0x8B, 0x00, 0xC5},
};

u8 sEffectColors[][3] = {
    {0x15, 0xD0, 0x25}, //Good
    {0xC0, 0x25, 0x35}, //Bad
};

struct PatchCard gAvailablePatches[MAX_CARDS];

struct PatchSelectionMenu gPatchSelectionMenu;

#ifdef DEBUG_PATCH_SELECT_MENU
const char testNameGood[] = {"Good Effect"};
const char testNameBad[] = {"Bad Effect"};
const char testDescGood[] = {"Good effect description"};
const char testDescBad[] = {"Bad effect description"};
const char testExtendedDesc[] = {"This is the extended description\nThere is more information here."};
#endif

/*
    Initializes the patch selection menu
*/
void init_patch_selection_menu() {
#ifdef DEBUG_PATCH_SELECT_MENU
    load_patch_card(0, 2, 0, 1, USE_TYPE_NONE, USE_TYPE_TIMED, testNameGood, testNameBad, testDescGood, testDescBad, testExtendedDesc, testExtendedDesc);
    load_patch_card(1, 3, 2, 4, USE_TYPE_LIMITED_USE, USE_TYPE_LIMITED_USE, testNameGood, testNameBad, testDescGood, testDescBad, testExtendedDesc, NULL);
    load_patch_card(2, 1, 10, 20, USE_TYPE_TIMED, USE_TYPE_LIMITED_USE, testNameGood, testNameBad, testDescGood, testDescBad, NULL, testExtendedDesc);
    load_patch_card(3, 3, 3, 16, USE_TYPE_LIMITED_USE, USE_TYPE_TIMED, testNameGood, testNameBad, testDescGood, testDescBad, NULL, NULL);
#else
    load_patch_card(0, 0, 0, 0, USE_TYPE_NONE, USE_TYPE_NONE, NULL, NULL, NULL, NULL, NULL, NULL);
    load_patch_card(0, 0, 0, 0, USE_TYPE_NONE, USE_TYPE_NONE, NULL, NULL, NULL, NULL, NULL, NULL);
    load_patch_card(0, 0, 0, 0, USE_TYPE_NONE, USE_TYPE_NONE, NULL, NULL, NULL, NULL, NULL, NULL);
    load_patch_card(0, 0, 0, 0, USE_TYPE_NONE, USE_TYPE_NONE, NULL, NULL, NULL, NULL, NULL, NULL);
#endif
    reset_patch_selection_menu();
}

/*
    Loads a patch card into gAvailablePatches in the slot specified by index
*/
void load_patch_card(s32 index, s32 quality, s32 effect1DurationOrUses, s32 effect2DurationOrUses, 
        s32 effect1UseType, s32 effect2UseType, const char *effect1Name, const char *effect2Name, 
        const char *effect1Desc, const char *effect2Desc, const char* effect1ExtendedDesc, const char *effect2ExtendedDesc) 
{
    assert(index < MAX_CARDS && index >= 0, "Tried to load a card into an invalid index!");
    gAvailablePatches[index].quality = quality;
    gAvailablePatches[index].patchDurationOrUses1 = effect1DurationOrUses;
    gAvailablePatches[index].patchDurationOrUses2 = effect2DurationOrUses;
    gAvailablePatches[index].patchUseType1 = effect1UseType;
    gAvailablePatches[index].patchUseType2 = effect2UseType;
    gAvailablePatches[index].patchName1 = effect1Name;
    gAvailablePatches[index].patchName2 = effect2Name;
    gAvailablePatches[index].patchDesc1 = effect1Desc;
    gAvailablePatches[index].patchDesc2 = effect2Desc;
    gAvailablePatches[index].extendedDesc1 = effect1ExtendedDesc;
    gAvailablePatches[index].extendedDesc2 = effect2ExtendedDesc;
    //TODO: Add an id or reference of some sort to an actual patch so it can be applied after selection
}

/*
    Updates the patch selection menu state and resets the animation timer
*/
void set_patch_selection_menu_state(u32 state) {
    gPatchSelectionMenu.menuState = state;
    gPatchSelectionMenu.animTimer = 0;
}

/*
    Resets the values of the patch selection menu to their defaults. Should always be called after the patch selection menu is exited.
*/
void reset_patch_selection_menu() {
    gPatchSelectionMenu.selectedPatch = 0;
    gPatchSelectionMenu.selectedMenuIndex = 0;
    gPatchSelectionMenu.flags = 0;
    gPatchSelectionMenu.framesSinceLastStickInput = 0;
    gPatchSelectionMenu.lastStickDir = JOYSTICK_MENU_DIR_NONE;
    gPatchSelectionMenu.menuState = PATCH_SELECT_STATE_STARTUP_ANIM;
    gPatchSelectionMenu.animTimer = 0;
    gPatchSelectionMenu.cardPos1[0] = CARD_X_LEFT_START;
    gPatchSelectionMenu.cardPos1[1] = CARD_Y_TOP;
    gPatchSelectionMenu.cardPos2[0] = CARD_X_RIGHT_START;
    gPatchSelectionMenu.cardPos2[1] = CARD_Y_TOP;
    gPatchSelectionMenu.cardPos3[0] = CARD_X_LEFT_START;
    gPatchSelectionMenu.cardPos3[1] = CARD_Y_BOTTOM;
    gPatchSelectionMenu.cardPos4[0] = CARD_X_RIGHT_START;
    gPatchSelectionMenu.cardPos4[1] = CARD_Y_BOTTOM;
    gPatchSelectionMenu.descPos[0] = PATCH_DESC_X;
    gPatchSelectionMenu.descPos[0] = PATCH_DESC_Y_START;
    gPatchSelectionMenu.curtainPos[0] = SCREEN_CENTER_X;
    gPatchSelectionMenu.curtainPos[1] = CURTAIN_Y_START;
    gPatchSelectionMenu.selectedCardScale = 1.05f;
    gPatchSelectionMenu.extendedDescScale = 0.0f;
}

/*
    Handles the player inputs for the patch selection state in the patch selection menu
*/
void handle_inputs_patch_select_state_select(s32 stickDir) {
    s32 previousSelection = gPatchSelectionMenu.selectedPatch;
    s32 selection = previousSelection;

    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
       set_patch_selection_menu_state(PATCH_SELECT_STATE_CONFIRMATION_TRANSITION_ANIM);
    } else if (gPlayer1Controller->buttonPressed & (Z_TRIG | L_TRIG)) {
        struct PatchCard *selectedCard = &gAvailablePatches[gPatchSelectionMenu.selectedPatch];
        if(selectedCard->extendedDesc1 || selectedCard->extendedDesc2) {
            set_patch_selection_menu_state(PATCH_SELECT_STATE_EXTENDED_DESC_APPEAR);
        }
    } else if(gPlayer1Controller->buttonPressed & D_JPAD || (stickDir == JOYSTICK_MENU_DIR_DOWN)) {
        selection += 2;
        if(selection > MAX_CARDS - 1) {
            selection = previousSelection;
        }
    } else if (gPlayer1Controller->buttonPressed & U_JPAD || (stickDir == JOYSTICK_MENU_DIR_UP)) {
        selection -= 2;
        if(selection < 0) {
            selection = previousSelection;
        }
    } else if(gPlayer1Controller->buttonPressed & R_JPAD || (stickDir == JOYSTICK_MENU_DIR_RIGHT)) {
        selection++;
        if(selection > MAX_CARDS - 1 || !(selection % 2)) {
            selection = previousSelection;
        }
    } else if(gPlayer1Controller->buttonPressed & L_JPAD || (stickDir == JOYSTICK_MENU_DIR_LEFT)) {
        selection--;
        if(selection < 0 || (selection % 2)) {
            selection = previousSelection;
        }
    }

    gPatchSelectionMenu.selectedPatch = selection;
}

/*
    Handles the player inputs for the extended description state in the patch selection menu
*/
void handle_inputs_patch_select_state_show_extended_desc() {
    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON | B_BUTTON | Z_TRIG | L_TRIG)) {
        set_patch_selection_menu_state(PATCH_SELECT_STATE_EXTENDED_DESC_DISAPPEAR);
    }
}

/*
    Handles the player inputs for the confirmation dialog state in the patch selection menu
*/
void handle_inputs_patch_select_state_confirmation(s32 stickDir) {
    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        if(gPatchSelectionMenu.selectedMenuIndex) {
            //No
            set_patch_selection_menu_state(PATCH_SELECT_STATE_CONFIRMATION_TRANSITION_RETURN_ANIM);
            gPatchSelectionMenu.selectedMenuIndex = 0;
        } else {
            //Yes
            set_patch_selection_menu_state(PATCH_SELECT_STATE_ENDING_ANIM);
        }
    } else if(gPlayer1Controller->buttonPressed & B_BUTTON) {
        set_patch_selection_menu_state(PATCH_SELECT_STATE_CONFIRMATION_TRANSITION_RETURN_ANIM);
        gPatchSelectionMenu.selectedMenuIndex = 0;
    }
    else if(gPlayer1Controller->buttonPressed & R_JPAD || (stickDir == JOYSTICK_MENU_DIR_RIGHT)) {
        gPatchSelectionMenu.selectedMenuIndex++;
        if(gPatchSelectionMenu.selectedMenuIndex > 1) {
            gPatchSelectionMenu.selectedMenuIndex = 0;
        }
    } else if(gPlayer1Controller->buttonPressed & L_JPAD || (stickDir == JOYSTICK_MENU_DIR_LEFT)) {
        gPatchSelectionMenu.selectedMenuIndex--;
        if(gPatchSelectionMenu.selectedMenuIndex < 0) {
            gPatchSelectionMenu.selectedMenuIndex = 1;
        }
    }
}

/*
    Handles joystick navigation for menus. Returns either the stick direction or JOYSTICK_MENU_DIR_NONE if 
    the direction is the same as the last direction, unless a certain number of frames has passed.
*/
u32 update_joystick_menu_dir() {
    f32 stickX = gPlayer1Controller->rawStickX;
    f32 stickY = gPlayer1Controller->rawStickY;
    u32 stickDir = JOYSTICK_MENU_DIR_NONE;

    if(stickY > 60) {
        stickDir = JOYSTICK_MENU_DIR_UP;
    } else if(stickY < -60) {
        stickDir = JOYSTICK_MENU_DIR_DOWN;
    } else if (stickX > 60) {
        stickDir = JOYSTICK_MENU_DIR_RIGHT;
    } else if (stickX < -60) {
        stickDir = JOYSTICK_MENU_DIR_LEFT;
    }

    if(gPatchSelectionMenu.framesSinceLastStickInput >= PATCH_SELECT_MENU_JOYSTICK_HOLD_FRAMES 
        || stickDir != gPatchSelectionMenu.lastStickDir)
    {
        gPatchSelectionMenu.lastStickDir = stickDir;
    } else {
        stickDir = JOYSTICK_MENU_DIR_NONE;
    }
    
    if(stickDir != JOYSTICK_MENU_DIR_NONE){
        gPatchSelectionMenu.framesSinceLastStickInput = 0;
    } else {
        if(gPatchSelectionMenu.framesSinceLastStickInput < PATCH_SELECT_MENU_JOYSTICK_HOLD_FRAMES) {
            gPatchSelectionMenu.framesSinceLastStickInput++;
        }
    }

    return stickDir;
}

/*
    Handles the player inputs for the patch selection menu
*/
void handle_patch_selection_inputs() {
    u32 stickDir = update_joystick_menu_dir();

    switch(gPatchSelectionMenu.menuState) {
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

/*
    Returns the point a percentage of the way from start to end
*/
f32 menu_translate_percentage(f32 start, f32 end, f32 percent) {
    f32 point = start + (end - start) * percent;
    return point;
}

#define PATCH_SELECT_STARTUP_CURTAIN_DROP_FRAMES        15
#define PATCH_SELECT_STARTUP_CARDS_SLIDE_FRAMES         9
/*
    Positions the menu elements to play the startup animation
*/
void patch_select_menu_startup_anim() {
    s16 curtainAnimTimer = gPatchSelectionMenu.animTimer;
    s16 cardsAnimTimer = 0;
    if (curtainAnimTimer >= PATCH_SELECT_STARTUP_CURTAIN_DROP_FRAMES) {
        cardsAnimTimer = gPatchSelectionMenu.animTimer - PATCH_SELECT_STARTUP_CURTAIN_DROP_FRAMES;
        curtainAnimTimer = PATCH_SELECT_STARTUP_CURTAIN_DROP_FRAMES;
        gPatchSelectionMenu.flags |= PATCH_SELECT_FLAG_STOP_GAME_RENDER;
    }
    f32 curtainPercent = (1.0f / PATCH_SELECT_STARTUP_CURTAIN_DROP_FRAMES) * curtainAnimTimer;
    f32 cardsPercent = (1.0f / PATCH_SELECT_STARTUP_CARDS_SLIDE_FRAMES) * cardsAnimTimer;

    f32 leftX = menu_translate_percentage(CARD_X_LEFT_START, CARD_X_LEFT, cardsPercent);
    f32 rightX = menu_translate_percentage(CARD_X_RIGHT_START, CARD_X_RIGHT, cardsPercent);
    f32 descY = menu_translate_percentage(PATCH_DESC_Y_START, PATCH_DESC_Y, cardsPercent);
    f32 curtainY = menu_translate_percentage(CURTAIN_Y_START, CURTAIN_Y_POS, curtainPercent);

    gPatchSelectionMenu.cardPos1[0] = leftX;
    gPatchSelectionMenu.cardPos1[1] = CARD_Y_TOP;
    
    gPatchSelectionMenu.cardPos2[0] = rightX;
    gPatchSelectionMenu.cardPos2[1] = CARD_Y_TOP;

    gPatchSelectionMenu.cardPos3[0] = leftX;
    gPatchSelectionMenu.cardPos3[1] = CARD_Y_BOTTOM;

    gPatchSelectionMenu.cardPos4[0] = rightX;
    gPatchSelectionMenu.cardPos4[1] = CARD_Y_BOTTOM;

    gPatchSelectionMenu.descPos[0] = PATCH_DESC_X;
    gPatchSelectionMenu.descPos[1] = descY;

    gPatchSelectionMenu.curtainPos[1] = curtainY;

    if(cardsAnimTimer >= PATCH_SELECT_STARTUP_CARDS_SLIDE_FRAMES) {
        set_patch_selection_menu_state(PATCH_SELECT_STATE_SELECT);
    }
}

#define PATCH_SELECT_ENDING_CARDS_SLIDE_FRAMES 9
#define PATCH_SELECT_ENDING_CURTAIN_RAISE_FRAMES 15
/*
    Positions the menu elements to play the ending animation (TEMP)
*/
void patch_select_menu_ending_anim() {
    s16 cardsAnimTimer = gPatchSelectionMenu.animTimer;
    s16 curtainAnimTimer = 0;
    if(cardsAnimTimer >= PATCH_SELECT_ENDING_CARDS_SLIDE_FRAMES) {
        cardsAnimTimer = PATCH_SELECT_ENDING_CARDS_SLIDE_FRAMES;
        curtainAnimTimer = gPatchSelectionMenu.animTimer - PATCH_SELECT_ENDING_CARDS_SLIDE_FRAMES;
        gPatchSelectionMenu.flags &= ~PATCH_SELECT_FLAG_STOP_GAME_RENDER;
    }
    f32 cardsPercent = (1.0f / PATCH_SELECT_ENDING_CARDS_SLIDE_FRAMES) * cardsAnimTimer;
    f32 curtainPercent = (1.0f / PATCH_SELECT_ENDING_CURTAIN_RAISE_FRAMES) * curtainAnimTimer;
    f32 targetX = menu_translate_percentage(PATCH_SELECTED_X, (CARD_X_RIGHT_START + 80), cardsPercent);
    f32 descY = menu_translate_percentage(PATCH_DESC_Y, PATCH_DESC_Y_START, cardsPercent);
    f32 curtainY = menu_translate_percentage(CURTAIN_Y_POS, CURTAIN_Y_START, curtainPercent);

    switch(gPatchSelectionMenu.selectedPatch) {
        case 0:
            gPatchSelectionMenu.cardPos1[0] = targetX;
            break;
        case 1:
            gPatchSelectionMenu.cardPos2[0] = targetX;
            break;
        case 2:
            gPatchSelectionMenu.cardPos3[0] = targetX;
            break;
        case 3:
            gPatchSelectionMenu.cardPos4[0] = targetX;
            break;
    }
    gPatchSelectionMenu.descPos[1] = descY;
    gPatchSelectionMenu.curtainPos[1] = curtainY;

    if(curtainAnimTimer >= PATCH_SELECT_ENDING_CURTAIN_RAISE_FRAMES) {
        set_patch_selection_menu_state(PATCH_SELECT_STATE_CLOSED);
    }
}

#define PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_ANIM_SCALE_FRAMES 7
#define PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_ANIM_SLIDE_FRAMES 5
/*
    Positions the menu elements to play the confirmation transition animation
*/
void patch_select_menu_confirmation_transition_anim() {
    s16 animTimer = gPatchSelectionMenu.animTimer;
    s16 scaleAnimTimer = 0;

    f32 completionPercentSlide = (1.0f / PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_ANIM_SLIDE_FRAMES) * animTimer;
    f32 leftX = menu_translate_percentage(CARD_X_LEFT, CARD_X_LEFT_START, completionPercentSlide);
    f32 rightX = menu_translate_percentage(CARD_X_RIGHT, CARD_X_RIGHT_START, completionPercentSlide);

    if(animTimer > PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_ANIM_SLIDE_FRAMES) {
        gPatchSelectionMenu.flags &= ~PATCH_SELECT_FLAG_DRAW_CURSOR;
        scaleAnimTimer = animTimer - PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_ANIM_SLIDE_FRAMES;
    } else {
        //Slide the cards off screen
        gPatchSelectionMenu.cardPos1[0] = leftX;
        gPatchSelectionMenu.cardPos2[0] = rightX;
        gPatchSelectionMenu.cardPos3[0] = leftX;
        gPatchSelectionMenu.cardPos4[0] = rightX;
    }

    f32 completionPercentScale = (1.0f / PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_ANIM_SCALE_FRAMES) * scaleAnimTimer;
    f32 selectedCardScale = menu_translate_percentage(1.05f, 1.75f, completionPercentScale);
    
    //Slide the selected card to the center of the screen
    switch(gPatchSelectionMenu.selectedPatch) {
        case 0:
            gPatchSelectionMenu.cardPos1[0] = menu_translate_percentage(CARD_X_LEFT, PATCH_SELECTED_X, completionPercentScale);
            gPatchSelectionMenu.cardPos1[1] = menu_translate_percentage(CARD_Y_TOP, PATCH_SELECTED_Y, completionPercentScale);
            break;
        case 1:
            gPatchSelectionMenu.cardPos2[0] = menu_translate_percentage(CARD_X_RIGHT, PATCH_SELECTED_X, completionPercentScale);
            gPatchSelectionMenu.cardPos2[1] = menu_translate_percentage(CARD_Y_TOP, PATCH_SELECTED_Y, completionPercentScale);
            break;
        case 2:
            gPatchSelectionMenu.cardPos3[0] = menu_translate_percentage(CARD_X_LEFT, PATCH_SELECTED_X, completionPercentScale);
            gPatchSelectionMenu.cardPos3[1] = menu_translate_percentage(CARD_Y_BOTTOM, PATCH_SELECTED_Y, completionPercentScale);
            break;
        case 3:
            gPatchSelectionMenu.cardPos4[0] = menu_translate_percentage(CARD_X_RIGHT, PATCH_SELECTED_X, completionPercentScale);
            gPatchSelectionMenu.cardPos4[1] = menu_translate_percentage(CARD_Y_BOTTOM, PATCH_SELECTED_Y, completionPercentScale);
            break;
    }

    gPatchSelectionMenu.selectedCardScale = selectedCardScale;

    if(scaleAnimTimer >= PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_ANIM_SCALE_FRAMES) {
        set_patch_selection_menu_state(PATCH_SELECT_STATE_CONFIRMATION);
    }
}

#define PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_RETURN_ANIM_SCALE_FRAMES 7
#define PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_RETURN_ANIM_SLIDE_FRAMES 5
/*
    Positions the menu elements to play the confirmation transition return animation
*/
void patch_select_menu_confirmation_return_transition_anim() {
    s16 animTimer = gPatchSelectionMenu.animTimer;
    s16 slideAnimTimer = 0;

    f32 completionPercentScale = (1.0f / PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_RETURN_ANIM_SCALE_FRAMES) * animTimer;
    f32 selectedCardScale = menu_translate_percentage(1.75f, 1.05f, completionPercentScale);

    if(animTimer > PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_RETURN_ANIM_SCALE_FRAMES) {
        slideAnimTimer = animTimer - PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_RETURN_ANIM_SCALE_FRAMES;
        completionPercentScale = 1.0f;
    } else {
        gPatchSelectionMenu.selectedCardScale = selectedCardScale;
    }

    f32 completionPercentSlide = (1.0f / PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_RETURN_ANIM_SLIDE_FRAMES) * slideAnimTimer;
    f32 leftX = menu_translate_percentage(CARD_X_LEFT_START, CARD_X_LEFT, completionPercentSlide);
    f32 rightX = menu_translate_percentage(CARD_X_RIGHT_START, CARD_X_RIGHT, completionPercentSlide);

    //Slide the cards on screen
    gPatchSelectionMenu.cardPos1[0] = leftX;
    gPatchSelectionMenu.cardPos2[0] = rightX;
    gPatchSelectionMenu.cardPos3[0] = leftX;
    gPatchSelectionMenu.cardPos4[0] = rightX;

    //Slide the selected card back from the center of the screen
    switch(gPatchSelectionMenu.selectedPatch) {
        case 0:
            gPatchSelectionMenu.cardPos1[0] = menu_translate_percentage(PATCH_SELECTED_X, CARD_X_LEFT, completionPercentScale);
            gPatchSelectionMenu.cardPos1[1] = menu_translate_percentage(PATCH_SELECTED_Y, CARD_Y_TOP, completionPercentScale);
            break;
        case 1:
            gPatchSelectionMenu.cardPos2[0] = menu_translate_percentage(PATCH_SELECTED_X, CARD_X_RIGHT, completionPercentScale);
            gPatchSelectionMenu.cardPos2[1] = menu_translate_percentage(PATCH_SELECTED_Y, CARD_Y_TOP, completionPercentScale);
            break;
        case 2:
            gPatchSelectionMenu.cardPos3[0] = menu_translate_percentage(PATCH_SELECTED_X, CARD_X_LEFT, completionPercentScale);
            gPatchSelectionMenu.cardPos3[1] = menu_translate_percentage(PATCH_SELECTED_Y, CARD_Y_BOTTOM, completionPercentScale);
            break;
        case 3:
            gPatchSelectionMenu.cardPos4[0] = menu_translate_percentage(PATCH_SELECTED_X, CARD_X_RIGHT, completionPercentScale);
            gPatchSelectionMenu.cardPos4[1] = menu_translate_percentage(PATCH_SELECTED_Y, CARD_Y_BOTTOM, completionPercentScale);
            break;
    }

    if(slideAnimTimer >= PATCH_SELECT_MENU_CONFIRMATION_TRANSITION_RETURN_ANIM_SLIDE_FRAMES) {
        set_patch_selection_menu_state(PATCH_SELECT_STATE_SELECT);
    }
}

#define PATCH_SELECT_MENU_EXT_DESC_APPEAR_ANIM_FRAMES   7
/*
    Updates the menu elements to play the extended decsription appearance animation
*/
void patch_select_menu_extended_desc_appear() {
    s16 animTimer = gPatchSelectionMenu.animTimer;
    f32 completionPercent = (1.0f / PATCH_SELECT_MENU_EXT_DESC_APPEAR_ANIM_FRAMES) * animTimer;
    gPatchSelectionMenu.flags |= PATCH_SELECT_FLAG_DRAW_EXTENDED_DESCRIPTION;

    f32 scale = menu_translate_percentage(0.0f, 1.0f, completionPercent);
    gPatchSelectionMenu.extendedDescScale = scale;

    if(animTimer >= PATCH_SELECT_MENU_EXT_DESC_APPEAR_ANIM_FRAMES) {
        set_patch_selection_menu_state(PATCH_SELECT_STATE_SHOW_EXTENDED_DESC);
    }
}

#define PATCH_SELECT_MENU_EXT_DESC_DISAPPEAR_ANIM_FRAMES   7
/*
    Updates the menu elements to play the extended decsription appearance animation
*/
void patch_select_menu_extended_desc_disappear() {
    s16 animTimer = gPatchSelectionMenu.animTimer;
    f32 completionPercent = (1.0f / PATCH_SELECT_MENU_EXT_DESC_DISAPPEAR_ANIM_FRAMES) * animTimer;

    f32 scale = menu_translate_percentage(1.0f, 0.0f, completionPercent);
    gPatchSelectionMenu.extendedDescScale = scale;
    
    if(animTimer >= PATCH_SELECT_MENU_EXT_DESC_DISAPPEAR_ANIM_FRAMES) {
        gPatchSelectionMenu.flags &= ~PATCH_SELECT_FLAG_DRAW_EXTENDED_DESCRIPTION;
        set_patch_selection_menu_state(PATCH_SELECT_STATE_SELECT);
    }
}

/*
    Updates the animations for the patch selection menu
*/
void update_patch_selection_menu_anims() {
    switch(gPatchSelectionMenu.menuState) {
        case PATCH_SELECT_STATE_STARTUP_ANIM:
            patch_select_menu_startup_anim();
            break;
        case PATCH_SELECT_STATE_SELECT:
            gPatchSelectionMenu.flags |= PATCH_SELECT_FLAG_DRAW_CURSOR;
            break;
        case PATCH_SELECT_STATE_CONFIRMATION_TRANSITION_ANIM:
            patch_select_menu_confirmation_transition_anim();
            break;
        case PATCH_SELECT_STATE_CONFIRMATION_TRANSITION_RETURN_ANIM:
            patch_select_menu_confirmation_return_transition_anim();
            break;
        case PATCH_SELECT_STATE_EXTENDED_DESC_APPEAR:
            patch_select_menu_extended_desc_appear();
            break;
        case PATCH_SELECT_STATE_EXTENDED_DESC_DISAPPEAR:
            patch_select_menu_extended_desc_disappear();
            break;
        case PATCH_SELECT_STATE_ENDING_ANIM:
            patch_select_menu_ending_anim();
            break;
    }
    gPatchSelectionMenu.animTimer++;
}

/*
    Updates the patch selection menu
*/
void update_patch_selection_menu() {
    handle_patch_selection_inputs();
    update_patch_selection_menu_anims();
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

void render_curtain_bg() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, gPatchSelectionMenu.curtainPos[0], gPatchSelectionMenu.curtainPos[1], 0);
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
    guTranslate(transMtx, -70, 28, 0);
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
void draw_patch_type(f32 x, f32 y, s32 type) {
    Mtx* transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    if(type == USE_TYPE_TIMED) {
        gSPDisplayList(gDisplayListHead++, star_timer);
    } else if (type == USE_TYPE_LIMITED_USE) {
        gSPDisplayList(gDisplayListHead++, uses_counter);
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws a patch card at the given x/y coordinates
*/
void render_patch_card(f32 x, f32 y, f32 scale, struct PatchCard *card, s32 reverse) {
    s32 quality = card->quality;
    s32 colorID = quality - 1;
    Mtx *cardScaleMtx = alloc_display_list(sizeof(Mtx));
    Mtx *cardTransMtx = alloc_display_list(sizeof(Mtx));
    char timer1Text[4];
    char timer2Text[4];

    gDPSetPrimColor(gDisplayListHead++, 0, 0, 
                    sQualityColors[colorID][0], sQualityColors[colorID][1], sQualityColors[colorID][2], 255);

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
    if(card->patchDurationOrUses1 > 0) {
        draw_patch_type(42, 15, card->patchUseType1);
        sprintf(timer1Text, "%d", card->patchDurationOrUses1);
    }
    if(card->patchDurationOrUses2 > 0) {
        draw_patch_type(42, -15, card->patchUseType2);
        sprintf(timer2Text, "%d", card->patchDurationOrUses2);
    }
    gSPDisplayList(gDisplayListHead++, patch_use_type_end);

    //Write text
    slowtext_setup_ortho_rendering(FT_FONT_SMALL_THIN);
    slowtext_draw_ortho_text(-63, 5, card->patchName1, FT_FLAG_ALIGN_LEFT, 
        sEffectColors[EFFECT_COLOR_GOOD][0], sEffectColors[EFFECT_COLOR_GOOD][1], sEffectColors[EFFECT_COLOR_GOOD][2], 0xFF);
    slowtext_draw_ortho_text(-63, -25, card->patchName2, FT_FLAG_ALIGN_LEFT, 
        sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2], 0xFF);
    slowtext_setup_ortho_rendering(FT_FONT_OUTLINE);
    if(card->patchDurationOrUses1 > 0) {
        slowtext_draw_ortho_text(51, 6, timer1Text, FT_FLAG_ALIGN_LEFT, 0xD0, 0xC4, 0x00, 0xFF);
    }if(card->patchDurationOrUses2 > 0) {
        slowtext_draw_ortho_text(51, -24, timer2Text, FT_FLAG_ALIGN_LEFT, 0xD0, 0xC4, 0x00, 0xFF);
    }
    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Writes the patch description
*/
void render_patch_desc() {
    s32 selected = gPatchSelectionMenu.selectedPatch;
    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(-142, 15, gAvailablePatches[selected].patchDesc1, FT_FLAG_ALIGN_LEFT, 
        sEffectColors[EFFECT_COLOR_GOOD][0], sEffectColors[EFFECT_COLOR_GOOD][1], sEffectColors[EFFECT_COLOR_GOOD][2], 0xFF);
    slowtext_draw_ortho_text(-142, -15, gAvailablePatches[selected].patchDesc2, FT_FLAG_ALIGN_LEFT, 
        sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2], 0xFF);
    slowtext_finished_rendering();
}

/*
    Draws a confirmation dialog
*/
void render_confirmation_dialog() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(0, 0, "Would you like to select this patch?", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(-30, -20, "Yes", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(30, -20, "No", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering();

    s32 selected = gPatchSelectionMenu.selectedMenuIndex;
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

    switch(gPatchSelectionMenu.menuState) {
        case PATCH_SELECT_STATE_CONFIRMATION:
            render_confirmation_dialog();
            break;
        case PATCH_SELECT_STATE_SELECT:
            render_patch_desc();
            break;
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
    if(gPatchSelectionMenu.menuState == PATCH_SELECT_STATE_CONFIRMATION_TRANSITION_ANIM) {
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
    s32 selected = gPatchSelectionMenu.selectedPatch;
    struct PatchCard *selectedCard = &gAvailablePatches[selected];
    f32 scale = gPatchSelectionMenu.extendedDescScale;
    
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
    if(selectedCard->extendedDesc1) {
        slowtext_draw_ortho_text(-142, 87, selectedCard->extendedDesc1, FT_FLAG_ALIGN_LEFT, 
            sEffectColors[EFFECT_COLOR_GOOD][0], sEffectColors[EFFECT_COLOR_GOOD][1], sEffectColors[EFFECT_COLOR_GOOD][2], 0xFF);
    }
    if(selectedCard->extendedDesc2) {
        //Draw second effect description lower if there are two extended descriptions
        if(selectedCard->extendedDesc1) {
            slowtext_draw_ortho_text(-142, -13, selectedCard->extendedDesc2, FT_FLAG_ALIGN_LEFT, 
                sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2], 0xFF);
        } else {
            slowtext_draw_ortho_text(-142, 87, selectedCard->extendedDesc2, FT_FLAG_ALIGN_LEFT, 
                sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2], 0xFF);
        }
    }
    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Displays the patch selection menu
*/
void display_patch_selection_ui() {
    s32 selectedPatch = gPatchSelectionMenu.selectedPatch;
    f32 cursorX, cursorY;
    f32 cardScale1 = 1.0f, cardScale2 = 1.0f, cardScale3 = 1.0f, cardScale4 = 1.0f;

    switch(selectedPatch) {
        case 0:
            cardScale1 = gPatchSelectionMenu.selectedCardScale;
            cursorX = CARD_X_LEFT;
            cursorY = CARD_Y_TOP;
            break;
        case 1:
            cardScale2 = gPatchSelectionMenu.selectedCardScale;
            cursorX = CARD_X_RIGHT;
            cursorY = CARD_Y_TOP;
            break;
        case 2:
            cardScale3 = gPatchSelectionMenu.selectedCardScale;
            cursorX = CARD_X_LEFT;
            cursorY = CARD_Y_BOTTOM;
            break;
        case 3:
        default:
            cardScale4 = gPatchSelectionMenu.selectedCardScale;
            cursorX = CARD_X_RIGHT;
            cursorY = CARD_Y_BOTTOM;
            break;
    }

    if(gPatchSelectionMenu.menuState != PATCH_SELECT_STATE_CLOSED) {
        patch_bg_scroll();
        desc_bg_scroll();
        create_dl_ortho_matrix(&gDisplayListHead);

        render_curtain_bg();
        render_patch_card(gPatchSelectionMenu.cardPos1[0], gPatchSelectionMenu.cardPos1[1], cardScale1, &gAvailablePatches[0], FALSE);
        render_patch_card(gPatchSelectionMenu.cardPos2[0], gPatchSelectionMenu.cardPos2[1], cardScale2, &gAvailablePatches[1], TRUE);
        render_patch_card(gPatchSelectionMenu.cardPos3[0], gPatchSelectionMenu.cardPos3[1], cardScale3, &gAvailablePatches[2], FALSE);
        render_patch_card(gPatchSelectionMenu.cardPos4[0], gPatchSelectionMenu.cardPos4[1], cardScale4, &gAvailablePatches[3], TRUE);
        render_lower_box(gPatchSelectionMenu.descPos[0], gPatchSelectionMenu.descPos[1]);

        if(gPatchSelectionMenu.flags & PATCH_SELECT_FLAG_DRAW_EXTENDED_DESCRIPTION) {
            render_extended_description();
        }

        if(gPatchSelectionMenu.flags & PATCH_SELECT_FLAG_DRAW_CURSOR) {
            render_patch_selection_cursor(cursorX, cursorY);
        }
    }
}