#pragma once

#include <PR/ultratypes.h>
#include "types.h"

#define DEFAULT_CARD_HEIGHT 200
#define DEFAULT_CARD_WIDTH  544
#define MAX_CARDS   4

struct PatchCard {
    u8 quality;
    const char patchName[];
};

struct PatchSelectionMenu {
    s8 selectedPatch;
    u8 isActive;
};

extern struct PatchSelectionMenu gPatchSelectionMenu;

void display_patch_selection_ui();
void handle_patch_selection_inputs();