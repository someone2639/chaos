#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "object_fields.h"
#include "game/level_update.h"
#include "behavior_data.h"
#include "game/chaos/chaos.h"
#include "game/object_list_processor.h"
#include "game/object_helpers.h"
#include "game/level_update.h"
#include "game/chaos/chaos_clone.h"
#include "engine/math_util.h"

struct Object *gMarioObjs[100];
struct MarioState *gMarioClones[100];

struct MarioState *gMarioSoul = &gMarioStates[0];
// struct Object *gMarioSoulObj = gMarioObject;

u32 gMarioCloneCount = 0;
u32 cspawnlock = 0;

s16 set_obj_animation(struct Object *o, s32 targetAnimID) {
    struct MarioState *m = gMarioSoul;
    // struct Object *
    struct Animation *targetAnim = m->animList->bufTarget;

    if (o->header.gfx.animInfo.animID != targetAnimID) {
        o->header.gfx.animInfo.animID = targetAnimID;
        o->header.gfx.animInfo.curAnim = targetAnim;
        o->header.gfx.animInfo.animAccel = 0;
        o->header.gfx.animInfo.animYTrans = m->unkB0;

        if (targetAnim->flags & ANIM_FLAG_2) {
            o->header.gfx.animInfo.animFrame = targetAnim->startFrame;
        } else {
            if (targetAnim->flags & ANIM_FLAG_FORWARD) {
                o->header.gfx.animInfo.animFrame = targetAnim->startFrame + 1;
            } else {
                o->header.gfx.animInfo.animFrame = targetAnim->startFrame - 1;
            }
        }
    }

    return o->header.gfx.animInfo.animFrame;
}

u32 cloneCount = 0;

u8 chs_cond_cherry_clone(void) {
    return (cloneCount < 10);
}

void chs_create_cherry_clone(void) {
    cloneCount ++;
}

void chs_remove_cherry_clone(void) {
    // TODO: despawn random clone
    cloneCount --;
}

void chs_init_cherry_clones_after_warp(void) {
    if (cspawnlock == 0) {
        cspawnlock = 1;
    } else {
        return;
    }

    s32 dAngle = 0x10000 / (cloneCount);

    for (s32 angle = 0; angle < 0xFFFF; angle += dAngle) {
        s16 dx = 300.0f * coss(angle);
        s16 dz = 300.0f * sins(angle);

        struct Object *newclone =
            spawn_object_relative(0,
                            dx, gMarioObject->header.gfx.pos[1], dz,
                            gMarioObject, MODEL_MARIO, bhvMarioClone
                                );
        newclone->header.gfx.sharedChild = gMarioObject->header.gfx.sharedChild;
        struct Object *prev = gCurrentObject;
        gCurrentObject = newclone;
        spawn_mist_particles();
        gCurrentObject = prev;
    }
}
