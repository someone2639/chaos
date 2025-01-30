#pragma once

#include "types.h"
#include "game/memory.h"

#define MAX_CLONES          4
#define CLONE_OFFSET_FRAMES 20
#define TOTAL_CLONE_FRAMES  (MAX_CLONES * CLONE_OFFSET_FRAMES)

#define oCosmicCloneTargetFrame     OBJECT_FIELD_VPTR(0x1B)
#define oCosmicCloneHandState       OBJECT_FIELD_S16(0x1C, 0)
#define oCosmicClonepunchState      OBJECT_FIELD_S16(0x1C, 1)

extern struct DmaHandlerList gCosmicClonesAnimsBuffs[];

struct CosmicCloneFrameData {
    Vec3f pos;
    Vec3s faceAngle;
    s8 active;
    s16 animId;
    s32 animAccel;
    s8 handState;
    u8 punchState;
};

Gfx *geo_switch_cosmic_clone_hand(s32 callContext, struct GraphNode *node, UNUSED Mat4 *c);
Gfx *geo_cosmic_clone_hand_foot_scaler(s32 callContext, struct GraphNode *node, UNUSED Mat4 *c);
void chs_cosmic_clones_move_instant_warp(Vec3s displacement);