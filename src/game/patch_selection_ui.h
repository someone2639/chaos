#pragma once

#include <PR/ultratypes.h>
#include "types.h"
#include "src/engine/math_util.h"

#define MAX_CARDS   4
#define MAX_QUALITY 3
#define EFFECT_COLOR_GOOD 0
#define EFFECT_COLOR_BAD 1

#define CARD_X_LEFT         ((SCREEN_WIDTH / 3) - 25)
#define CARD_X_RIGHT        (((SCREEN_WIDTH / 3) * 2) + 25)
#define CARD_Y_TOP          (SCREEN_HEIGHT - 45)
#define CARD_Y_BOTTOM       (SCREEN_HEIGHT / 2)
#define PATCH_DESC_X        (SCREEN_WIDTH / 2)
#define PATCH_DESC_Y        42

#define CARD_X_LEFT_START -85
#define CARD_X_RIGHT_START (SCREEN_WIDTH + 85)
#define CARD_1_X_START      CARD_X_LEFT_START
#define CARD_2_X_START      (CARD_X_RIGHT_START + 100)
#define CARD_3_X_START      (CARD_X_LEFT_START - 200)
#define CARD_4_X_START      (CARD_X_RIGHT_START + 300)
#define CARD_X_END          (CARD_X_RIGHT_START + 80)

#define CARD_SCALE_DEFAULT  1.0f
#define CARD_SCALE_HOVER    1.05f
#define CARD_SCALE_SELECTED 1.75f

#define PATCH_DESC_Y_START  -50

#define PATCH_SELECTED_X   (SCREEN_WIDTH / 2)
#define PATCH_SELECTED_Y   (SCREEN_CENTER_Y + 40)

#define CURTAIN_Y_POS      SCREEN_CENTER_Y
#define CURTAIN_Y_START    (SCREEN_HEIGHT + 160)

#define SELECT_PATCH_TEXT_END   -80

#define PATCH_SELECT_MENU_JOYSTICK_HOLD_FRAMES 10

enum JoystickMenuDir {
    JOYSTICK_MENU_DIR_NONE,
    JOYSTICK_MENU_DIR_UP,
    JOYSTICK_MENU_DIR_DOWN,
    JOYSTICK_MENU_DIR_RIGHT,
    JOYSTICK_MENU_DIR_LEFT,
};

struct PatchCard {
    u8 quality;
    u16 patchDurationOrUses1;
    u16 patchDurationOrUses2;
    u8 patchUseType1;
    u8 patchUseType2;
    const char *patchName1;
    const char *patchName2;
    const char *patchDesc1;
    const char *patchDesc2;
    const char *extendedDesc1;
    const char *extendedDesc2;
    Vec2f pos;
    f32 scale;
    //TODO: Add an id or reference of some sort to an actual patch so it can be applied after selection
};

enum Patch_Select_Menu_State {
    PATCH_SELECT_STATE_STARTUP_ANIM,
    PATCH_SELECT_STATE_SELECT,
    PATCH_SELECT_STATE_CONFIRMATION_TRANSITION_ANIM,
    PATCH_SELECT_STATE_CONFIRMATION_TRANSITION_RETURN_ANIM,
    PATCH_SELECT_STATE_CONFIRMATION,
    PATCH_SELECT_STATE_EXTENDED_DESC_APPEAR,
    PATCH_SELECT_STATE_EXTENDED_DESC_DISAPPEAR,
    PATCH_SELECT_STATE_SHOW_EXTENDED_DESC,
    PATCH_SELECT_STATE_ENDING_ANIM,
    PATCH_SELECT_STATE_CLOSED,
};

enum PatchCardUseType {
    USE_TYPE_NONE,
    USE_TYPE_TIMED,
    USE_TYPE_LIMITED_USE,
};

enum PatchSelectionMenuFlags {
    PATCH_SELECT_FLAG_ACTIVE                        = (1 << 0),
    PATCH_SELECT_FLAG_STOP_GAME_RENDER              = (1 << 1),
    PATCH_SELECT_FLAG_DRAW_EXTENDED_DESCRIPTION     = (1 << 2),
    PATCH_SELECT_FLAG_DRAW_CURSOR                   = (1 << 3),
    PATCH_SELECT_FLAG_DRAW_START_TEXT               = (1 << 4),
};

struct PatchSelectionMenu {
    s8 selectedPatch;
    s8 selectedMenuIndex;
    u32 flags;
    s16 framesSinceLastStickInput;
    u32 lastStickDir;
    u32 menuState;
    u16 animTimer;
    u8 animPhase;
    Vec2f descPos;
    Vec2f curtainPos;
    Vec2f selectPatchTextPos;
    f32 extendedDescScale;
    f32 selectPatchTextScale;
};

extern struct PatchSelectionMenu gPatchSelectionMenu;
extern struct PatchCard gAvailablePatches[];

void display_patch_selection_ui();
void update_patch_selection_menu();
void reset_patch_selection_menu();
void load_patch_card(s32 index, s32 quality, s32 effect1DurationOrUses, s32 effect2DurationOrUses, 
        s32 effect1UseType, s32 effect2UseType, const char *effect1Name, const char *effect2Name, 
        const char *effect1Desc, const char *effect2Desc, const char* effect1ExtendedDesc, const char *effect2ExtendedDesc);
void init_patch_selection_menu();