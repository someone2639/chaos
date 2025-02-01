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
#include "game/object_list_processor.h"

/*
    Green Demon
*/
u8 chs_cond_green_demon(void) {
    return (!chaos_check_if_patch_active(CHAOS_PATCH_SPEED_LIMIT) && !chaos_check_if_patch_active(CHAOS_PATCH_WALKIES));
}

void chs_act_green_demon(void) {
    if(!gMarioObject) {
        return;
    }

    if(gCurrCourseNum != COURSE_NONE) {
        spawn_object_abs_with_rot(gMarioState->marioObj, 0, MODEL_GREEN_DEMON, bhvGreenDemon,
                            gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2], 0, 0, 0);
    }
}

void chs_area_init_green_demon(void) {
    spawn_object_abs_with_rot(gMarioState->marioObj, 0, MODEL_GREEN_DEMON, bhvGreenDemon,
                            gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2], 0, 0, 0);
}

/*
    Bullet Hell
*/

#define BULLET_TYPE_STRAIGHT    0
#define BULLET_TYPE_SPIRAL      1
#define BULLET_TYPE_HOMING      2
#define BULLET_TYPE_SINE        3
#define BULLET_TYPE_INVSINE     4
#define BULLET_TYPE_FALLING     5

u8 sNextPattern = 0;
s16 sBulletCooldown = -1;

//8 bills in a circle around Mario 4 times
u32 bullet_pattern_1() {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_BULLET_HELL, &this);
    if(this->frameTimer % 60) {
        return FALSE;
    }
    f32 marioX = gMarioState->pos[0];
    f32 marioY = gMarioState->pos[1];
    f32 marioZ = gMarioState->pos[2];
    f32 billX;
    f32 billY;
    f32 billZ;
    f32 billYaw = 0;
    for(int i = 0; i < 8; i++) {
        billX = marioX + (1000 * sins(0x1FFF * i));
        billY = marioY + 125;
        billZ = marioZ + (1000 * coss(0x1FFF * i));
        billYaw = ((0xFFFF / 8) * i) + 0x8000;
        struct Object *bullet = spawn_object_abs_with_rot(gMarioState->marioObj, 0, MODEL_CHAOS_BULLET_BILL, 
                                                        bhvChaosBulletBill, billX, billY, billZ, 0, billYaw, 0);
        bullet->oBehParams2ndByte = BULLET_TYPE_STRAIGHT;
        bullet->oForwardVel = 40.0f;
        bullet->oChaosBillTTL = 60;
    }
    if(this->frameTimer > 180) {
        return TRUE;
    } else {
        return FALSE;
    }
}

//Several homing shots offset by one second each
u32 bullet_pattern_2() {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_BULLET_HELL, &this);
    if(this->frameTimer % 30) {
        return FALSE;
    }

    s16 spawnOffset;

    if(this->frameTimer % 60) {
        spawnOffset = 0x2000;
    } else {
        spawnOffset = -0x2000;
    }

    f32 marioX = gMarioState->pos[0];
    f32 marioY = gMarioState->pos[1];
    f32 marioZ = gMarioState->pos[2];

    f32 billYaw = gMarioState->faceAngle[1] - spawnOffset; 
    f32 billX = marioX + (1500 * sins(gMarioState->faceAngle[1] + spawnOffset));
    f32 billY = marioY + 125;
    f32 billZ = marioZ + (1500 * coss(gMarioState->faceAngle[1] + spawnOffset));
    
    struct Object *bullet = spawn_object_abs_with_rot(gMarioState->marioObj, 0, MODEL_CHAOS_BULLET_BILL, 
                                                    bhvChaosBulletBill, billX, billY, billZ, 0, billYaw, 0);

    bullet->oBehParams2ndByte = BULLET_TYPE_HOMING;
    bullet->oForwardVel = 25.0f;
    bullet->oChaosBillTTL = 150;

    if(this->frameTimer > 120) {
        return TRUE;
    } else {
        return FALSE;
    }
}

//Falling in random pattern
u32 bullet_pattern_3() {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_BULLET_HELL, &this);
    if(this->frameTimer % 5) {
        return FALSE;
    }

    f32 marioX = gMarioState->pos[0];
    f32 marioY = gMarioState->pos[1];
    f32 marioZ = gMarioState->pos[2];

    f32 dist = RAND(750);
    s16 angle = RAND(0xFFFF);

    f32 billYaw = gMarioState->faceAngle[1]; 
    f32 billX = marioX + (dist * sins(angle));
    f32 billY = marioY + 1500;
    f32 billZ = marioZ + (dist * coss(angle));
    
    struct Object *bullet = spawn_object_abs_with_rot(gMarioState->marioObj, 0, MODEL_CHAOS_BULLET_BILL, 
                                                    bhvChaosBulletBill, billX, billY, billZ, 0, billYaw, 0);

    bullet->oBehParams2ndByte = BULLET_TYPE_FALLING;
    
    bullet->oVelY = -40.0f;
    bullet->oForwardVel = 0.0f;
    bullet->oChaosBillTTL = 75;

    if(this->frameTimer > 150) {
        return TRUE;
    } else {
        return FALSE;
    }
}

//Rapid patterns alternating between x and + shapes
u32 bullet_pattern_4() {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_BULLET_HELL, &this);
    if(this->frameTimer % 30) {
        return FALSE;
    }
    f32 marioX = gMarioState->pos[0];
    f32 marioY = gMarioState->pos[1];
    f32 marioZ = gMarioState->pos[2];
    f32 billX;
    f32 billY;
    f32 billZ;
    f32 billYaw = 0;
    s16 offset;

    if(this->frameTimer % 30) {
        offset = 0x2000;
    } else {
        offset = 0;
    }

    for(int i = 0; i < 4; i++) {
        billX = marioX + (1500 * sins((0x3FFF * i) + offset));
        billY = marioY + 125;
        billZ = marioZ + (1500 * coss((0x3FFF * i) + offset));
        billYaw = ((0xFFFF / 4) * i) + 0x8000 + offset;
        struct Object *bullet = spawn_object_abs_with_rot(gMarioState->marioObj, 0, MODEL_CHAOS_BULLET_BILL, 
                                                        bhvChaosBulletBill, billX, billY, billZ, 0, billYaw, 0);
        bullet->oBehParams2ndByte = BULLET_TYPE_STRAIGHT;
        bullet->oForwardVel = 50.0f;
        bullet->oChaosBillTTL = 30;
    }
    if(this->frameTimer > 180) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
    One burst of long lasting sine pattern bullets
*/
u32 bullet_pattern_5() {
    f32 marioX = gMarioState->pos[0];
    f32 marioY = gMarioState->pos[1];
    f32 marioZ = gMarioState->pos[2];
    f32 billX;
    f32 billY;
    f32 billZ;
    f32 billYaw = 0;
    s16 offset;
    s16 angOffset;
    s32 type;

    for(int i = 0; i < 8; i++) {
        if(i % 2) {
            offset = 0x1000;
            type = BULLET_TYPE_SINE;
        } else {
            offset = -0x1000;
            type = BULLET_TYPE_INVSINE;
        }
        angOffset = 0x4000 * (i/2);

        billYaw = (gMarioState->faceAngle[1] - 0x8000) + angOffset; 
        billX = marioX + (1500 * sins(gMarioState->faceAngle[1] + offset + angOffset));
        billY = marioY + 125;
        billZ = marioZ + (1500 * coss(gMarioState->faceAngle[1] + offset + angOffset));

        struct Object *bullet = spawn_object_abs_with_rot(gMarioState->marioObj, 0, MODEL_CHAOS_BULLET_BILL, 
                                                        bhvChaosBulletBill, billX, billY, billZ, 0, billYaw, 0);
        bullet->oBehParams2ndByte = type;
        bullet->oForwardVel = 60.0f;
        bullet->oChaosBillTTL = 180;
    }

    return TRUE;
}

/*
    3 rounds of spirals around Mario
*/
u32 bullet_pattern_6() {
    f32 marioX = gMarioState->pos[0];
    f32 marioY = gMarioState->pos[1];
    f32 marioZ = gMarioState->pos[2];
    f32 billX;
    f32 billY;
    f32 billZ;
    f32 billYaw = 0;
    s16 offset;

    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_BULLET_HELL, &this);

    if(this->frameTimer % 60) {
        return FALSE;
    }

    for(int i = 0; i < 4; i++) {
        offset = 0x2000 + (0x4000 * i);
        billYaw = (gMarioState->faceAngle[1]) + offset; 
        billX = marioX + (800 * sins(gMarioState->faceAngle[1] + offset));
        billY = marioY + 125;
        billZ = marioZ + (800 * coss(gMarioState->faceAngle[1] + offset));

        struct Object *bullet = spawn_object_abs_with_rot(gMarioState->marioObj, 0, MODEL_CHAOS_BULLET_BILL, 
                                                        bhvChaosBulletBill, billX, billY, billZ, 0, billYaw, 0);
        bullet->oBehParams2ndByte = BULLET_TYPE_SPIRAL;
        bullet->oForwardVel = 100.0f;
        bullet->oChaosBillTTL = 35;
    }

    if(this->frameTimer == 180) {
        return TRUE;
    } else {
        return FALSE;
    }
}

#define CHS_NUM_HELL_PATTERNS   6
u32 (*sChaosBulletPatterns[CHS_NUM_HELL_PATTERNS])(void) = {
    &bullet_pattern_1,
    &bullet_pattern_2,
    &bullet_pattern_3,
    &bullet_pattern_4,
    &bullet_pattern_5,
    &bullet_pattern_6,
};

void chs_reset_bullet_hell() {
    sNextPattern = RAND(CHS_NUM_HELL_PATTERNS);
    sBulletCooldown = 900;
}

void chs_area_init_bullet_hell(void) {
    chs_reset_bullet_hell();
}

void chs_update_bullet_hell(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_BULLET_HELL, &this);

    u32 (*pattern)(void) = sChaosBulletPatterns[sNextPattern];

    if(gCurrCourseNum == COURSE_NONE) {
        sBulletCooldown = -1;
    }
    
    switch(sBulletCooldown) {
        case 45:
        case 30:
        case 15:
            play_sound(SOUND_MOVING_ALMOST_DROWNING, gGlobalSoundSource);
            break;
        case 0:
            if(pattern()) {
                chs_reset_bullet_hell();
            }
            break;
    }
    if (sBulletCooldown > 0) {
        this->frameTimer = 0;
        sBulletCooldown--;
    }
}

void chs_lvlinit_spawn_on_shell(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_SPAWN_ON_SHELL, &this);

    this->frameTimer = 0;
}

void chs_lvlupdate_spawn_on_shell(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_SPAWN_ON_SHELL, &this);

    if (gCurrCourseNum == COURSE_NONE 
        || gCurrLevelNum == LEVEL_BOWSER_1
        || gCurrLevelNum == LEVEL_BOWSER_2
        || gCurrLevelNum == LEVEL_BOWSER_3) {
        this->frameTimer = 0xFFFFFF;
        return;
    }

    if (this->frameTimer < 0xFFFFFF && !(gMarioState->action & ACT_FLAG_RIDING_SHELL) && gMarioState->marioObj) {
        struct Object *obj = spawn_object(gMarioState->marioObj, MODEL_KOOPA_SHELL, bhvKoopaShell);
        gMarioState->interactObj = obj;
        gMarioState->usedObj = obj;
        gMarioState->riddenObj = obj;
        obj->oInteractStatus = ATTACK_FAST_ATTACK + (INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED);
        update_mario_sound_and_camera(gMarioState);
        play_shell_music();
        mario_drop_held_object(gMarioState);

        set_mario_action(gMarioState, ACT_RIDING_SHELL_FALL, 0);
        this->frameTimer = 0xFFFFFF;
    }
}
