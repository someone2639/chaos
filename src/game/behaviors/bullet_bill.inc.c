// bullet_bill.inc.c

void bhv_white_puff_smoke_init(void) {
    cur_obj_scale(random_float() * 2 + 2.0);
}

void bhv_bullet_bill_init(void) {
    o->oBulletBillInitialMoveYaw = o->oMoveAngleYaw;
}

void bullet_bill_act_0(void) {
    cur_obj_become_tangible();
    o->oForwardVel = 0.0f;
    o->oMoveAngleYaw = o->oBulletBillInitialMoveYaw;
    o->oFaceAnglePitch = 0;
    o->oFaceAngleRoll = 0;
    o->oMoveFlags = 0;
    cur_obj_set_pos_to_home();
    o->oAction = 1;
}

void bullet_bill_act_1(void) {
    s16 sp1E = abs_angle_diff(o->oAngleToMario, o->oMoveAngleYaw);
    if (sp1E < 0x2000 && 400.0f < o->oDistanceToMario && o->oDistanceToMario < 1500.0f) {
        o->oAction = 2;
    }
}

void bullet_bill_act_2(void) {
    if (o->oTimer < 40) {
        o->oForwardVel = 3.0f;
    } else if (o->oTimer < 50) {
        if (o->oTimer % 2) {
            o->oForwardVel = 3.0f;
        } else {
            o->oForwardVel = -3.0f;
        }
    } else {
        if (o->oTimer > 70) {
            cur_obj_update_floor_and_walls();
        }

        spawn_object(o, MODEL_SMOKE, bhvWhitePuffSmoke);
        o->oForwardVel = 30.0f;

        if (o->oDistanceToMario > 300.0f) {
            cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x100);
        }

        if (o->oTimer == 50) {
            cur_obj_play_sound_2(SOUND_OBJ_POUNDING_CANNON);
            cur_obj_shake_screen(SHAKE_POS_SMALL);
        }

        if (o->oTimer > 150 || o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
            o->oAction = 3;
            spawn_mist_particles();
        }
    }
}

void bullet_bill_act_3(void) {
    o->oAction = 0;
}

void bullet_bill_act_4(void) {
    if (o->oTimer == 0) {
        o->oForwardVel = -30.0f;
        cur_obj_become_intangible();
    }

    o->oFaceAnglePitch += 0x1000;
    o->oFaceAngleRoll += 0x1000;
    o->oPosY += 20.0f;

    if (o->oTimer > 90) {
        o->oAction = 0;
    }
}

void (*sBulletBillActions[])(void) = {
    bullet_bill_act_0,
    bullet_bill_act_1,
    bullet_bill_act_2,
    bullet_bill_act_3,
    bullet_bill_act_4,
};

void bhv_bullet_bill_loop(void) {
    cur_obj_call_action_function(sBulletBillActions);
    if (cur_obj_check_interacted()) {
        o->oAction = 4;
    }
}

//Straight line
void chaos_bullet_hell_type_1(){
    if(o->oTimer > o->oChaosBillTTL) {
        obj_mark_for_deletion(o);
    }
}

//Spiral
void chaos_bullet_hell_type_2(){
    o->oMoveAngleYaw += 0x600;
    o->oForwardVel *= 1.01f;
    if(o->oTimer > o->oChaosBillTTL) {
        obj_mark_for_deletion(o);
    }
}

//Homing
void chaos_bullet_hell_type_3(){
    f32 dist;
    s16 pitch;
    s16 yaw;
    Vec3f oPos;
    oPos[0] = o->oPosX;
    oPos[1] = o->oPosY;
    oPos[2] = o->oPosZ;
    vec3f_get_dist_and_angle(oPos, gMarioState->pos, &dist, &pitch, &yaw);
    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, yaw, 0x400);
    o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, -pitch, 0x400);
    o->oVelY = -sins(o->oFaceAnglePitch) * o->oForwardVel;

    if(o->oTimer > o->oChaosBillTTL) {
        obj_mark_for_deletion(o);
    }
}

//Sine
void chaos_bullet_hell_type_4(){
    if(o->oTimer == 0) {
        o->oBulletBillInitialMoveYaw = o->oMoveAngleYaw;
    }
    o->oMoveAngleYaw = o->oBulletBillInitialMoveYaw + (0x4000 * (sins(((0xFFFF / 60) * o->oTimer))));
    if(o->oTimer > o->oChaosBillTTL) {
        obj_mark_for_deletion(o);
    }
}

//Inverse sine
void chaos_bullet_hell_type_5(){
    if(o->oTimer == 0) {
        o->oBulletBillInitialMoveYaw = o->oMoveAngleYaw;
    }
    o->oMoveAngleYaw = o->oBulletBillInitialMoveYaw + (0x4000 * (sins(((0xFFFF / 60) * o->oTimer) + 0x8000)));
    if(o->oTimer > o->oChaosBillTTL) {
        obj_mark_for_deletion(o);
    }
}

//Falling
void chaos_bullet_hell_type_6() {
    o->oFaceAnglePitch = 0x4000;
    if(o->oTimer > o->oChaosBillTTL) {
        obj_mark_for_deletion(o);
    }
}

void (*sChaosBulletActions[])(void) = {
    &chaos_bullet_hell_type_1,
    &chaos_bullet_hell_type_2,
    &chaos_bullet_hell_type_3,
    &chaos_bullet_hell_type_4,
    &chaos_bullet_hell_type_5,
    &chaos_bullet_hell_type_6,
};

void bhv_chaos_bullet_bill_loop(void) {
    if(o->oTimer == 0) {
        cur_obj_play_sound_2(SOUND_OBJ_POUNDING_CANNON);
        cur_obj_shake_screen(SHAKE_POS_SMALL);
    }
    void (*action)(void) = sChaosBulletActions[o->oBehParams2ndByte];
    action();
}
