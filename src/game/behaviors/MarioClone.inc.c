
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

struct Object *obj_get_collider(struct Object *obj) {
    s32 numCollidedObjs;
    struct Object *other;
    // s32 touchedOtherObject = FALSE;

    numCollidedObjs = obj->numCollidedObjs;
    if (numCollidedObjs != 0) {
        other = obj->collidedObjs[0];
    }

    return other;
}

void force_mario_interaction(struct MarioState *m, struct Object *obj) {
    struct InteractionHandler *ih;

    for (int i = 0; i < INTERACT_COUNT; i++) {
        ih = &sInteractionHandlers[i];
        if (obj->collidedObjInteractTypes & ih->interactType) {
            m->collidedObjInteractTypes |= ih->interactType;
            // obj->collidedObjInteractTypes &= ~ih->interactType;
            // break;
        }
    }

    // ih->handler(m, ih->interactType, o);
}

void update_clone_animation() {
    struct MarioState *m = gMarioState;
    s32 newAnim = m->marioObj->header.gfx.animInfo.animID;
    struct Animation *targetAnim = m->animList->bufTarget;

    if (o->header.gfx.animInfo.animID != newAnim) {
        o->header.gfx.animInfo.animID = newAnim;
    }

    o->header.gfx.animInfo.animFrame = m->marioObj->header.gfx.animInfo.animFrame;
}

void swap(struct MarioState *m, struct Object *obj) {
    Vec3f tempPos;
    Vec3f tempVel;

    vec3f_copy(tempPos, m->pos);
    vec3f_copy(m->pos, &obj->oPosX);
    vec3f_copy(&obj->oPosX, tempPos);
}

void bhv_MarioClone_init(void) {
	set_obj_animation(o, MARIO_ANIM_IDLE_HEAD_LEFT);
}
void bhv_MarioClone_loop(void) {
    char debug[220];
	struct MarioState *m = gMarioState;
    obj_set_hitbox(o, &sCloneHitbox);
    if (obj_attack_collided_from_other_object(o)) {
		// forward this to mario
        struct Object *coll = obj_get_collider(o);
        coll->oInteractStatus = 0;

        if ((coll != gMarioObject) && (coll->behavior != segmented_to_virtual(bhvMarioClone))) {
            swap(m, o);
    		m->interactObj = coll;
            force_mario_interaction(m, m->interactObj);
        }
		o->oInteractStatus = 0;
	}
	update_clone_animation();

	o->oFaceAnglePitch = m->faceAngle[0];
	o->oFaceAngleYaw = m->faceAngle[1];
	o->oFaceAngleRoll = m->faceAngle[2];

	vec3f_copy(&o->oVelX, m->vel);

	u32 collisionFlags = object_step();

    if (!(m->action & ACT_FLAG_AIR)) {
        vec3f_add(&o->oPosX, &o->oVelX);
    }
    o->oForwardVel = m->forwardVel;

    // sprintf(debug, "V %0.2f %0.2f %0.2f P %0.2f %0.2f %0.2f\n", o->oVelX, o->oVelY, o->oVelZ, 
    //     o->oPosX, o->oPosY, o->oPosZ);
    // osSyncPrintf(debug);

}
