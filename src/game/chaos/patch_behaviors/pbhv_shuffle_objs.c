#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "behavior_data.h"
#include "game/mario.h"
#include "sm64.h"
#include "object_fields.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "game/object_list_processor.h"
#include "game/object_helpers.h"
#include "behavior_data.h"
#include "game/chaos/chaos.h"
#include "engine/behavior_script.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"

f32 posStore[OBJECT_POOL_CAPACITY][3] = { 0 };
struct Object *shuffleList[OBJECT_POOL_CAPACITY] = {0};
u32 shuffleCount = 0;

void cshuffle_reset(UNUSED struct Camera *c) {
    shuffleCount = 0;
    for (int i = 0; i < OBJECT_POOL_CAPACITY; i++) {
        shuffleList[i] = NULL;
        posStore[i][0] = 0;
        posStore[i][1] = 0;
        posStore[i][2] = 0;
    }
}

void cshuffle_populate_shuffle_list(UNUSED struct Camera *c) {
    // start at 1 to skip mario
    for (int ol = 1; ol < NUM_OBJ_LISTS; ol++) {
        if (ol == OBJ_LIST_SURFACE) continue;
        struct Object *head = (struct Object *)&gObjectLists[ol];
        if (head == NULL) continue;

        struct Object *o = head;

        u32 i = shuffleCount;
        do {
            if (o->behavior == segmented_to_virtual(bhvStaticObject)) {
                o = (struct Object *)o->header.next;
                continue;
            }
            shuffleList[i] = o;

            posStore[i][0] = o->oPosX;
            posStore[i][1] = o->oPosY;
            posStore[i][2] = o->oPosZ;

            o = (struct Object *)o->header.next;
            i++;
        } while (o != head);
        shuffleCount = i;
    }

    for (int i = (int)shuffleCount - 1; i >= 0; i--) {
        s32 randIndex = random_float() * (i + 1);
        struct Object *tmp = shuffleList[randIndex];
        shuffleList[randIndex] = shuffleList[i];
        shuffleList[i] = tmp;
    }
}

void cshuffle_approach(UNUSED struct Camera *c) {
    for (u32 i = 0; i < shuffleCount; i++) {
        struct Object *o = shuffleList[i];
        if (gMarioObject == o) continue;

        #define FACTOR 0.25f
        o->oPosX = approach_f32_asymptotic(o->oPosX, posStore[i][0], FACTOR);
        o->oPosY = approach_f32_asymptotic(o->oPosY, posStore[i][1], FACTOR);
        o->oPosZ = approach_f32_asymptotic(o->oPosZ, posStore[i][2], FACTOR);

        o->header.gfx.pos[0] = approach_f32_asymptotic(o->header.gfx.pos[0], posStore[i][0], FACTOR);
        o->header.gfx.pos[1] = approach_f32_asymptotic(o->header.gfx.pos[1], posStore[i][1], FACTOR);
        o->header.gfx.pos[2] = approach_f32_asymptotic(o->header.gfx.pos[2], posStore[i][2], FACTOR);
    }
}

void cshuffle_setfloors(UNUSED struct Camera *c) {
    for (u32 i = 0; i < shuffleCount; i++) {
        struct Object *o = shuffleList[i];
        if (gMarioObject == o) continue;

        if (o->oMoveFlags & OBJ_MOVE_ON_GROUND) {
            f32 floor = find_floor_height(o->oPosX, o->oPosY + 200.0f, o->oPosZ);
            posStore[i][1] = floor;
        }
    }
}

void cshuffle_timestop(UNUSED struct Camera *c) {
    enable_time_stop_including_mario();
}
void cshuffle_timestart(UNUSED struct Camera *c) {
    disable_time_stop_including_mario();
}

extern void cutscene_goto_cvar_pos(struct Camera *c, f32 goalDist, s16 goalPitch, s16 rotPitch, s16 rotYaw);
extern s16 sStatusFlags;
void cutscene_shuffle(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    cutscene_event(cshuffle_reset, c, 0, 1);
    cutscene_event(cshuffle_timestop, c, 0, 1);
    cutscene_event(cshuffle_populate_shuffle_list, c, 0, 1);
    cutscene_event(cshuffle_approach, c, 30, -1);
    cutscene_event(cshuffle_setfloors, c, 59, -1);
    cutscene_event(cshuffle_timestart, c, 59, -1);

    // set pos (TODO: give a level overview instead of a random angle relative to mario)
    #define CPOS 0
    c->pos[0] = CPOS;
    c->pos[1] = CPOS;
    c->pos[2] = CPOS;

    // set focus
    vec3f_copy(c->focus, gMarioState->pos);
    // c->focus[0] = 0;
    // c->focus[0] = 0;
    // c->focus[0] = 0;
}

void cutscene_shuffle_end(struct Camera *c) {
    c->cutscene = 0;
}

struct Cutscene sCutsceneShuffleObjs[] = {
    {cutscene_shuffle, 60},
    {cutscene_shuffle_end, 0},
};


void chs_shuffle_objs_init(void) {
    gMarioState->statusForCamera->cameraEvent = CAM_EVENT_SHUFFLE;
}
