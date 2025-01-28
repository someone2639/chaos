#include "game/chaos/patch_behaviors/pbhv_cosmic_clones.h"
#include "game/chaos/chaos_patch_behaviors.h"

struct ObjectHitbox sCosmicClonesHitbox = {
    /* interactType:      */ INTERACT_DAMAGE,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 3,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 37,
    /* height:            */ 160,
    /* hurtboxRadius:     */ 37,
    /* hurtboxHeight:     */ 160,
};

s16 set_cosmic_clone_animation(s32 targetAnimID, s32 accel) {
    struct Animation *targetAnim = gCosmicClonesAnimsBuffs[o->oBehParams2ndByte].bufTarget;

    if (load_patchable_table(&gCosmicClonesAnimsBuffs[o->oBehParams2ndByte], targetAnimID)) {
        targetAnim->values = (void *) VIRTUAL_TO_PHYSICAL((u8 *) targetAnim + (uintptr_t) targetAnim->values);
        targetAnim->index = (void *) VIRTUAL_TO_PHYSICAL((u8 *) targetAnim + (uintptr_t) targetAnim->index);
    }

    if (o->header.gfx.animInfo.animID != targetAnimID) {
        o->header.gfx.animInfo.animID = targetAnimID;
        o->header.gfx.animInfo.curAnim = targetAnim;
        o->header.gfx.animInfo.animAccel = accel;
        o->header.gfx.animInfo.animYTrans = gMarioState->unkB0;
        

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

void cosmic_clone_copy_frame_data() {
    struct CosmicCloneFrameData *cloneFrame = (struct CosmicCloneFrameData *)o->oCosmicCloneTargetFrame;

    o->oPosX = cloneFrame->pos[0];
    o->oPosY = cloneFrame->pos[1];
    o->oPosZ = cloneFrame->pos[2];

    o->oFaceAnglePitch = cloneFrame->faceAngle[0];
    o->oFaceAngleYaw = cloneFrame->faceAngle[1];
    o->oFaceAngleRoll = cloneFrame->faceAngle[2];

    o->oCosmicCloneHandState = cloneFrame->handState;
    o->oCosmicClonepunchState = cloneFrame->punchState;

    set_cosmic_clone_animation(cloneFrame->animId, cloneFrame->animAccel);
}

void bhv_cosmic_clones_init(void) {
    obj_set_hitbox(o, &sCosmicClonesHitbox);
}

void bhv_cosmic_clones_loop(void) {
    struct CosmicCloneFrameData *cloneFrame = (struct CosmicCloneFrameData *)o->oCosmicCloneTargetFrame;

    if(!chaos_check_if_patch_active(CHAOS_PATCH_COSMIC_CLONES)) {
        spawn_mist_particles_with_sound(SOUND_OBJ_DEFAULT_DEATH);
        obj_mark_for_deletion(o);
        return;
    }

    switch(gMarioState->action) {
        //I think this is all of the death actions
        case ACT_DEATH_ON_BACK:
        case ACT_DEATH_ON_STOMACH:
        case ACT_STANDING_DEATH:
        case ACT_QUICKSAND_DEATH:
        case ACT_WATER_DEATH:
        case ACT_DROWNING:
        //And star grab
        case ACT_STAR_DANCE_EXIT:
        case ACT_STAR_DANCE_WATER:
        case ACT_FALL_AFTER_STAR_GRAB:
        case ACT_STAR_DANCE_NO_EXIT:
            spawn_mist_particles_with_sound(SOUND_OBJ_DEFAULT_DEATH);
            obj_mark_for_deletion(o);
            break;
    }

    switch(o->oAction) {
        case 0:
            // Wait for frame to be active
            o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
            o->oIntangibleTimer = -1;
            if(cloneFrame->active) {
                cosmic_clone_copy_frame_data();

                o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
                o->oIntangibleTimer = 0;

                spawn_mist_particles_with_sound(SOUND_OBJ_DEFAULT_DEATH);

                o->oAction++;
            }
            break;
        case 1:
            // Copy frame data
            cosmic_clone_copy_frame_data();
            //Don't hurt Mario when metal
            if(gMarioState->flags & MARIO_METAL_CAP || chs_check_temporary_invincibility()) {
                o->oIntangibleTimer = 2;
            }
            break;
    }

    if ((o->oInteractStatus & INT_STATUS_INTERACTED)) {
        o->oInteractStatus = 0;
    }
}
