
#include "game/chaos/chaos_clone.h"

struct Object *obj_get_collider(struct Object *obj) {
    s32 numCollidedObjs;
    struct Object *other;
    s32 touchedOtherObject = FALSE;

    numCollidedObjs = obj->numCollidedObjs;
    if (numCollidedObjs != 0) {
        other = obj->collidedObjs[0];

        if (other != gMarioObject) {
            other->oInteractStatus |= ATTACK_PUNCH | INT_STATUS_WAS_ATTACKED | INT_STATUS_INTERACTED
                                      | INT_STATUS_TOUCHED_BOB_OMB;
            touchedOtherObject = TRUE;
        }
    }

    return other;
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

void bhv_MarioClone_init(void) {
	set_obj_animation(o, MARIO_ANIM_IDLE_HEAD_LEFT);
}
void bhv_MarioClone_loop(void) {
	if (o->oInteractStatus & INT_STATUS_INTERACTED) {
		// forward this to mario
		gMarioState->interactObj = obj_get_collider(o);
		o->oInteractStatus = 0;
	}


	o->oFaceAnglePitch = gMarioState->faceAngle[0];
	o->oFaceAngleYaw = gMarioState->faceAngle[1];
	o->oFaceAngleRoll = gMarioState->faceAngle[2];

	vec3f_copy(&o->oVelX, gMarioState->vel);
	o->oForwardVel = gMarioState->forwardVel;

	u32 collisionFlags = object_step();

	cur_obj_update_floor_and_walls();

	update_clone_animation();
}
