#pragma once

#include <PR/ultratypes.h>
#include "types.h"

#define MAX_CARDS   4
#define MAX_QUALITY 4

struct PatchCard {
    u8 quality;
    u16 duration;
    const char patchName[];
};

struct PatchSelectionMenu {
    s8 selectedPatch;
    u8 isActive;
};

extern struct PatchSelectionMenu gPatchSelectionMenu;

void display_patch_selection_ui();
void handle_patch_selection_inputs();