#pragma once

#include <PR/ultratypes.h>
#include "types.h"

#define DEFAULT_CARD_HEIGHT 200

extern s32 sSelectedPatch;

struct PatchCard {
    u8 quality;
    const char patchName[];
};

void display_patch_selection_ui();