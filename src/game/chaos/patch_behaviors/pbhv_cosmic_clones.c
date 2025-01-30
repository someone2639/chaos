#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/level_update.h"
#include "behavior_data.h"
#include "game/interaction.h"
#include "game/object_helpers.h"
#include "game/print.h"
#include "game/game_init.h"
#include "game/sound_init.h"
#include "engine/math_util.h"
#include "game/chaos/chaos.h"
#include "game/area.h"
#include "course_table.h"
#include "audio/external.h"
#include "game/memory.h"
#include "game/debug.h"
#include "game/rendering_graph_node.h"
#include "mario_geo_switch_case_ids.h"

#include "pbhv_cosmic_clones.h"

Vec3f sMarioSpawnLocation;
s8  sStartClones = FALSE;

static s8 gCosmicCloneAttackScaleAnimation[3 * 6] = {
    10, 12, 16, 24, 10, 10, 10, 14, 20, 30, 10, 10, 10, 16, 20, 26, 26, 20,
};

struct CosmicCloneFrameData gCosmicCloneFrames[TOTAL_CLONE_FRAMES];

void spawn_cosmic_clones(s32 startActive) {
    u32 model = (chaos_check_if_patch_active(CHAOS_PATCH_LUIGI)) ? MODEL_CLONE_LUIGI : MODEL_CLONE_MARIO;

    for(int i = 0; i < MAX_CLONES; i++) {
        struct Object *clone = spawn_object_abs_with_rot(gMarioState->marioObj, 0, model, bhvCosmicClone,
                                gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2], 0, 0, 0);
        clone->oCosmicCloneTargetFrame = &gCosmicCloneFrames[(CLONE_OFFSET_FRAMES * (i + 1)) - 1];
        clone->oBehParams2ndByte = i;
        if(startActive) {
            clone->oAction = 1;
        }
    }
}

void chs_act_cosmic_clones(void) {
    //In case the patch is activated in the middle of a course
    if(gCurrCourseNum != COURSE_NONE) {
        spawn_cosmic_clones(FALSE);
        bzero(gCosmicCloneFrames, sizeof(struct CosmicCloneFrameData) * TOTAL_CLONE_FRAMES);
        vec3f_copy(sMarioSpawnLocation, gMarioState->pos);
        sStartClones = FALSE;
    }
}

void chs_area_init_cosmic_clones(void) {
    spawn_cosmic_clones(FALSE);
    bzero(gCosmicCloneFrames, sizeof(struct CosmicCloneFrameData) * TOTAL_CLONE_FRAMES);
    vec3f_copy(sMarioSpawnLocation, gMarioState->pos);

    sStartClones = FALSE;
}

void chs_update_cosmic_clones(void) {
    if(gCurrCourseNum == COURSE_NONE) { 
        return;
    }
    // Wait for Mario to move away from the starting position, as determined by when he exits the landing animation
    // In theory this should prevent Mario from getting instantly hit upon entering the stage.
    if(!sStartClones) {
        if(gMarioState->action & ACT_GROUP_CUTSCENE) {
            vec3f_copy(sMarioSpawnLocation, gMarioState->pos);
        }

        f32 x = gMarioState->pos[0] - sMarioSpawnLocation[0];
        f32 y = gMarioState->pos[1] - sMarioSpawnLocation[1];
        f32 z = gMarioState->pos[2] - sMarioSpawnLocation[2];
        f32 marioDistFromSpawn = sqrtf(x * x + y * y + z * z);

        if(marioDistFromSpawn > 500.0f) {
            sStartClones = TRUE;
        }

        return;
    }
    
    for(int i = TOTAL_CLONE_FRAMES - 1; i > 0; i--) {
        bcopy(&gCosmicCloneFrames[i - 1], &gCosmicCloneFrames[i], sizeof(struct CosmicCloneFrameData));
    }

    struct Object *marioObj = gMarioState->marioObj;

    vec3f_copy(gCosmicCloneFrames[0].pos, marioObj->header.gfx.pos);
    vec3s_copy(gCosmicCloneFrames[0].faceAngle, marioObj->header.gfx.angle);

    gCosmicCloneFrames[0].animId = marioObj->header.gfx.animInfo.animID;
    gCosmicCloneFrames[0].animAccel = marioObj->header.gfx.animInfo.animAccel;

    if(gMarioState->marioBodyState->action & ACT_FLAG_SWIMMING_OR_FLYING) {
        gCosmicCloneFrames[0].handState = MARIO_HAND_OPEN;
    } else {
        gCosmicCloneFrames[0].handState = MARIO_HAND_FISTS;
    }
    gCosmicCloneFrames[0].punchState = gMarioState->marioBodyState->punchState;

    gCosmicCloneFrames[0].active = TRUE;
}

void chs_deact_cosmic_clones(void) {
    bzero(gCosmicCloneFrames, sizeof(struct CosmicCloneFrameData) * TOTAL_CLONE_FRAMES);
}

Gfx *geo_switch_cosmic_clone_hand(s32 callContext, struct GraphNode *node, UNUSED Mat4 *c) {
    struct GraphNodeSwitchCase *switchCase = (struct GraphNodeSwitchCase *) node;
    struct Object *nodeObj = (struct Object *) gCurGraphNodeObject;

    if (callContext == GEO_CONTEXT_RENDER) {
        if (switchCase->numCases == 0) {
            switchCase->selectedCase =
                (nodeObj->oCosmicCloneHandState < 2) ? nodeObj->oCosmicCloneHandState : MARIO_HAND_OPEN;
        } else {
            switchCase->selectedCase =
                (nodeObj->oCosmicCloneHandState < 2) ? nodeObj->oCosmicCloneHandState : MARIO_HAND_FISTS;
        }
    }
        
    return NULL;
}

Gfx *geo_cosmic_clone_hand_foot_scaler(s32 callContext, struct GraphNode *node, UNUSED Mat4 *c) {
    struct GraphNodeGenerated *asGenerated = (struct GraphNodeGenerated *) node;
    struct GraphNodeScale *scaleNode = (struct GraphNodeScale *) node->next;
    struct Object *nodeObj = (struct Object *) gCurGraphNodeObject;

    if (callContext == GEO_CONTEXT_RENDER) {
        scaleNode->scale = 1.0f;
        if (asGenerated->parameter == (u32)nodeObj->oCosmicClonepunchState >> 6) {
            scaleNode->scale =
                gCosmicCloneAttackScaleAnimation[asGenerated->parameter * 6 + (nodeObj->oCosmicClonepunchState & 0x3F)]
                / 10.0f;
        }
    }
    return NULL;
}

void chs_cosmic_clones_move_instant_warp(Vec3s displacement) {
    spawn_cosmic_clones(TRUE);
    for(int i = 0; i < TOTAL_CLONE_FRAMES; i++) {
        gCosmicCloneFrames[i].pos[0] += displacement[0];
        gCosmicCloneFrames[i].pos[1] += displacement[1];
        gCosmicCloneFrames[i].pos[2] += displacement[2];
    }
}