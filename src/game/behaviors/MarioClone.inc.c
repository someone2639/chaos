
#include "game/chaos/chaos_clone.h"
#include "game/interaction.h"

static struct ObjectHitbox sCloneHitbox = {
    /* interactType:      */ INTERACT_GRABBABLE,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 0,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 50,
    /* height:            */ 157,
    /* hurtboxRadius:     */ 0,
    /* hurtboxHeight:     */ 0,
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

    if (o->header.gfx.animInfo.animID != newAnim) {
        o->header.gfx.animInfo.animID = newAnim;
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
	o->oMoveAngleYaw = m->intendedYaw;
    o->oFaceAngleYaw = m->intendedYaw;
	o->oFaceAngleRoll = m->faceAngle[2];

	vec3f_copy(&o->oVelX, m->vel);

	UNUSED u32 collisionFlags = object_step();

    if (!(m->action & ACT_FLAG_AIR)) {
        vec3f_add(&o->oPosX, &o->oVelX);
    }
    o->oForwardVel = m->forwardVel;
    o->header.gfx.sharedChild = gMarioObject->header.gfx.sharedChild;

    struct Surface *floor = NULL;
    f32 floorheight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &floor);
    if (floor) {
        if (floor->type == SURFACE_BURNING) {
            if ((ABS(o->oPosY - floorheight) < 10)) {
                delete_clone(o);
            }
        }
    }
}
