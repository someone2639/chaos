#pragma once

#include <PR/ultratypes.h>
#include "types.h"

#define MAX_CARDS   4
#define MAX_QUALITY 3

#define CARD_X_LEFT         (SCREEN_WIDTH / 3) - 25
#define CARD_X_RIGHT        ((SCREEN_WIDTH / 3) * 2) + 25
#define CARD_Y_TOP          SCREEN_HEIGHT - 45
#define CARD_Y_BOTTOM       (SCREEN_HEIGHT / 2)
#define PATCH_DESC_X        SCREEN_WIDTH / 2
#define PATCH_DESC_Y        42

struct PatchCard {
    u8 quality;
    u16 duration1;
    u16 duration2;
    const char *patchName1;
    const char *patchName2;
    const char *patchDesc1;
    const char *patchDesc2;
    const char *extendedDesc1;
    const char *extendedDesc2;
    //TODO: Add an id or reference of some sort to an actual patch so it can be applied after selection
};

enum Patch_Select_Menu_State {
    STATE_SELECT,
    STATE_CONFIRMATION,
    STATE_SHOW_EXTENDED_DESC,
    STATE_CLOSED,
};

struct PatchSelectionMenu {
    s8 selectedPatch;
    s8 selectedMenuIndex;
    u8 isActive;
    s16 framesSinceLastStickInput;
    u32 menuState;
};

extern struct PatchSelectionMenu gPatchSelectionMenu;
extern struct PatchCard gAvailablePatches[];

void display_patch_selection_ui();
void handle_patch_selection_inputs();
void reset_patch_selection_menu();
void load_patch_card(s32 index, s32 quality, s32 effect1Duration, s32 effect2Duration, 
        const char *effect1Name, const char *effect2Name, const char *effect1Desc, const char *effect2Desc, 
        const char* effect1ExtendedDesc, const char *effect2ExtendedDesc);
void init_patch_selection_menu();