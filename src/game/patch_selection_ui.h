#pragma once

#include <PR/ultratypes.h>
#include "types.h"

#define MAX_CARDS   4
#define MAX_QUALITY 4

#define CARD_X_LEFT         (SCREEN_WIDTH / 3) - 25
#define CARD_X_RIGHT        ((SCREEN_WIDTH / 3) * 2) + 25
#define CARD_Y_TOP          SCREEN_HEIGHT - 45
#define CARD_Y_BOTTOM       (SCREEN_HEIGHT / 2)
#define PATCH_DESC_X        SCREEN_WIDTH / 2
#define PATCH_DESC_Y        42

struct PatchCard {
    u8 quality;
    u16 duration;
    const char *patchName1;
    const char *patchName2;
    const char *patchDesc1;
    const char *patchDesc2;
};

struct PatchSelectionMenu {
    s8 selectedPatch;
    u8 isActive;
};

extern struct PatchSelectionMenu gPatchSelectionMenu;

void display_patch_selection_ui();
void handle_patch_selection_inputs();