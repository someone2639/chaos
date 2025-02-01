
#include "game/chaos/chaos_clone.h"
#include "game/interaction.h"

static struct ObjectHitbox sCloneHitbox = {
    /* interactType:      */ 0,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 0,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 37,
    /* height:            */ 160,
    /* hurtboxRadius:     */ 37,
    /* hurtboxHeight:     */ 160,
};

extern u32 cspawnlock;

struct Object *obj_get_collider(struct Object *obj) {
    s32 numCollidedObjs;
    struct Object *other = NULL;
    // s32 touchedOtherObject = FALSE;

    numCollidedObjs = obj->numCollidedObjs;
    if (numCollidedObjs != 0) {
        other = obj->collidedObjs[0];
    }

    return other;
}

void swap(struct MarioState *m, struct Object *obj) {
    Vec3f tempPos;
    // Vec3f tempVel;

    vec3f_copy(tempPos, m->pos);
    vec3f_copy(m->pos, &obj->oPosX);
    vec3f_copy(&obj->oPosX, tempPos);
}

void force_mario_interaction(struct MarioState *m, struct Object *obj) {
    struct InteractionHandler *ih;

    for (int i = 0; i < INTERACT_COUNT; i++) {
        ih = &sInteractionHandlers[i];
        if (obj->collidedObjInteractTypes & ih->interactType) {
            if (!(ih->interactType & (INTERACT_DAMAGE | INTERACT_FLAME))) {
                m->collidedObjInteractTypes |= ih->interactType;
                obj->collidedObjInteractTypes &= ~ih->interactType;
            } else {
                obj->collidedObjInteractTypes |= ih->interactType;
            }
        }
    }
}

void update_clone_animation() {
    struct MarioState *m = gMarioState;
    s32 newAnim = m->marioObj->header.gfx.animInfo.animID;
    struct Animation *targetAnim = m->animList->bufTarget;

    if (o->header.gfx.animInfo.animID != newAnim) {
        o->header.gfx.animInfo.animID = newAnim;

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

    o->header.gfx.animInfo.animFrame = m->marioObj->header.gfx.animInfo.animFrame;
}

void bhv_MarioClone_init(void) {
    cspawnlock = 0;
	set_obj_animation(o, MARIO_ANIM_IDLE_HEAD_LEFT);
}

void delete_clone(struct Object *obj) {
    obj_mark_for_deletion(obj);
    spawn_mist_particles();
}

struct MarioState fakeMario;

void bhv_MarioClone_loop(void) {
	struct MarioState *m = gMarioState;
    obj_set_hitbox(o, &sCloneHitbox);

    if (o->oAction == 50) {
        delete_clone(o);
    }

    if (obj_attack_collided_from_other_object(o)) {
		// forward this to mario
        struct Object *coll = obj_get_collider(o);
        coll->oInteractStatus = 0;

        if ((coll != gMarioObject) && (coll->behavior != segmented_to_virtual(bhvMarioClone))) {
            m->interactObj = coll;
            force_mario_interaction(m, m->interactObj);
            if ((coll->oDamageOrCoinValue > 0) && (coll->oInteractType != INTERACT_COIN)) {
                delete_clone(o);
            } else {
                swap(m, o);
            }
        }

		o->oInteractStatus = 0;
	}
	update_clone_animation();

    o->oFaceAnglePitch = m->faceAngle[0];
    o->oFaceAngleYaw = m->faceAngle[1];
    o->oFaceAngleRoll = m->faceAngle[2];

    o->oGravity = 1.0f;

	UNUSED u32 collisionFlags = object_step();
    if (m->action & ACT_FLAG_ON_POLE) {
        o->oVelY = 0;
    } else {
        o->oPosX += m->vel[0];
        o->oPosZ += m->vel[2];
    }
    if (m->vel[1] != 0) {
        o->oVelY = m->vel[1];
    }

    o->header.gfx.sharedChild = gMarioObject->header.gfx.sharedChild;

    struct Surface *floor = NULL;
    f32 floorheight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &floor);
    if (!floor) {
        floorheight = find_floor(o->oPosX, o->oPosY + 100.0f, o->oPosZ, &floor);
        if (!floor) {
            delete_clone(o);
        } else {
            o->oPosY = floorheight;
        }
    } else {
        if (floor->type == SURFACE_BURNING) {
            if ((ABS(o->oPosY - floorheight) < 10)) {
                delete_clone(o);
            }
        }

        if (floor->type == SURFACE_DEATH_PLANE) {
            if (o->oPosY < (floorheight + 2048.0f)) {
                delete_clone(o);
            }
        }
    }

    struct Surface *ceil = NULL;
    f32 ceilheight = find_ceil(o->oPosX, o->oPosY, o->oPosZ, &ceil);
    if (ceil && floor) {
        if ((floorheight + 160.0f) > ceilheight) {
            delete_clone(o);
        }
    }

    // char rr[100];
    // sprintf(rr, "VELY %f\n", o->oVelY);
    // osSyncPrintf(rr);
}
