#pragma once

#include <PR/ultratypes.h>
#include "types.h"
#include "chaos/chaos.h"

struct ChaosStats {
    enum ChaosDifficulty difficulty;
    enum ChaosGameMode gameMode;
    u8 stars;
    u8 blueStars;
    u16 deaths;
    u16 gameLoads;
    u16 totalPatches;
    u32 playTime;
    u8 menuTimer;
    struct ChaosActiveEntry finalPatches[CHAOS_PATCH_ENTRIES]; // Annoyingly redundant, but what can you do...
    u16 finalPatchCount;
};

void init_chaos_stats(void);
void draw_chaos_stats(Gfx **dl);
s32 update_chaos_stats(void);