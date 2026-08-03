// poser.inc.c

void set_poser_animation(void) {
    struct Animation *targetAnim = gMarioState->animList->bufTarget;

    o->header.gfx.animInfo.animID = gMarioObject->header.gfx.animInfo.animID;
    o->header.gfx.animInfo.curAnim = targetAnim;
    o->header.gfx.animInfo.animAccel = 1;
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

void bhv_poser_init(void) {
    set_poser_animation();
}

void bhv_poser_loop(void) {
    if (!chaos_check_if_patch_active(CHAOS_PATCH_POSER)) {
        obj_mark_for_deletion(o);
        return;
    }

    o->oPosX = gMarioObject->oPosX;
    o->oPosY = gMarioObject->oPosY;
    o->oPosZ = gMarioObject->oPosZ;

    o->oFaceAnglePitch = gMarioObject->oFaceAnglePitch;
    o->oFaceAngleYaw = gMarioObject->oFaceAngleYaw;
    o->oFaceAngleRoll = gMarioObject->oFaceAngleRoll;

    o->header.gfx.scale[0] = gMarioObject->header.gfx.scale[0];
    o->header.gfx.scale[1] = gMarioObject->header.gfx.scale[1];
    o->header.gfx.scale[2] = gMarioObject->header.gfx.scale[2];

    o->header.gfx.node.flags = gMarioObject->header.gfx.node.flags;
}
