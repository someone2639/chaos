#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
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

s16 cloneyaw = 0;

void chs_create_clone(void) {
    Vec3f dest;
    Vec3f zero = {0};
    vec3f_set_dist_and_angle(zero, dest, 300.0f, 0, cloneyaw);
    cloneyaw += 0x2000;
    struct Object *newclone = spawn_object_relative(0,
                                                    dest[0], dest[1], dest[2],
                                                    gMarioObject, MODEL_MARIO, bhvMarioClone
                                                    );

    struct Object *prev = gCurrentObject;
    gCurrentObject = newclone;
    spawn_mist_particles();
    gCurrentObject = prev;
}
