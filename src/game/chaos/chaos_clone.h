#pragma once

extern struct Object *gMarioObjs[100];
extern struct MarioState *gMarioClones[100];

extern struct MarioState *gMarioSoul;
extern struct Object *gMarioSoulObj;

extern u32 gMarioCloneCount;

s16 set_obj_animation(struct Object *o, s32 targetAnimID);
