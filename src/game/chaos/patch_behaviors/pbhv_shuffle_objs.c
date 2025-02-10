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
#include "course_table.h"

// #define SKIPWARPS
#define SKIP_LEVELOBJS
#define SET_HOMES

f32 posStore[OBJECT_POOL_CAPACITY][3] = { 0 };
struct Object *shuffleList[OBJECT_POOL_CAPACITY] = {0};
u32 shuffleCount = 0;

#define STARCOUNT 10
f32 starPos[STARCOUNT][3] = { 0 };
struct Object *starList[STARCOUNT] = {0};
u32 starCount = 0;

const BehaviorScript *surflist[] = {
    bhvExclamationBox,
    bhvMessagePanel,
    bhvWoodenPost,
    // bhvChainChompGate,
    bhvSeesawPlatform,
    bhvCheckerboardPlatformSub,
    bhvThwomp,
    bhvHorizontalGrindel,
    bhvThwomp2,
    bhvGrindel,
    bhvCapSwitch,


    // TTC memes
    bhvTTCRotatingSolid,
    bhvTTCSpinner,
};

const BehaviorScript *ignorelist[] = {
    bhvStar,
    bhvUkiki,
    bhvInitializeChangingWaterLevel,
    bhvWaterLevelDiamond,
};

const BehaviorScript *allowedWithParents[] = {
    bhvTreasureChestBottom,
    bhvRedCoin,
};

u8 isInList(struct Object *o, const BehaviorScript *list[], int count) {
    for (int i = 0; i < count; i++) {
        if (o->behavior == segmented_to_virtual(list[i])) {
            return 1;
        }
    }

    return 0;
}

#define IS_INLIST(o, list) isInList(o, list, ARRAY_COUNT(list))

u8 isBeingShuffled(struct Object *o) {
    for (u32 i = 0; i < shuffleCount; i++) {
        if (o == shuffleList[i]) {
            return 1;
        }
    }

    return 0;
}

void cshuffle_reset(UNUSED struct Camera *c) {
    shuffleCount = 0; starCount = 0;
    for (int i = 0; i < OBJECT_POOL_CAPACITY; i++) {
        shuffleList[i] = NULL;
        posStore[i][0] = 0;
        posStore[i][1] = 0;
        posStore[i][2] = 0;
    }
    for (int i = 0; i < STARCOUNT; i++) {
        starList[i] = NULL;
        starPos[i][0] = 0;
        starPos[i][1] = 0;
        starPos[i][2] = 0;
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
#ifdef SKIP_LEVELOBJS
            if (o->behavior == segmented_to_virtual(bhvStaticObject)) {
                o = (struct Object *)o->header.next;
                continue;
            }
#endif // SKIP_LEVELOBJS
            if (IS_INLIST(o, ignorelist)) {
                o = (struct Object *)o->header.next;
                continue;
            }
#ifdef SKIPWARPS
            if (o->behavior == segmented_to_virtual(bhvWarp)) {
                o = (struct Object *)o->header.next;
                continue;
            }
#endif // SKIPWARPS
            if ((o->parentObj != o) && (!IS_INLIST(o, allowedWithParents))) {
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
}

void cshuffle_populate_star_list(UNUSED struct Camera *c) {
    // add stars
    struct Object *head = (struct Object *)&gObjectLists[OBJ_LIST_LEVEL];
    struct Object *o = head;
    u32 i = 0;
    do {
        if (o->behavior != segmented_to_virtual(bhvStar)) {
            o = (struct Object *)o->header.next;
            continue;
        }
        starList[i] = o;

        starPos[i][0] = o->oPosX;
        starPos[i][1] = o->oPosY;
        starPos[i][2] = o->oPosZ;

        o = (struct Object *)o->header.next;
        i++;
    } while (o != head);
}


void cshuffle_populate_surface_list(UNUSED struct Camera *c) {
    // start at 1 to skip mario
    u32 ol = OBJ_LIST_SURFACE;
    struct Object *head = (struct Object *)&gObjectLists[ol];
    if (head == NULL) return;

    struct Object *o = head;

    u32 i = shuffleCount;
    do {
        if (!IS_INLIST(o, surflist)) {
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

void cshuffle_sort(UNUSED struct Camera *c) {
    struct Object *el0 = shuffleList[0];
    for (int i = (int)shuffleCount - 1; i > 0; i--) {
        s32 randIndex;
        do {
            randIndex = random_u16() % (i + 1);
        } while (randIndex == i);
        struct Object *tmp = shuffleList[randIndex];
        shuffleList[randIndex] = shuffleList[i];
        shuffleList[i] = tmp;
    }
    if (el0 == shuffleList[0]) {
        struct Object *swap = shuffleList[0];
        shuffleList[0] = shuffleList[1];
        shuffleList[1] = swap;
    }

    for (int i = (int)starCount - 1; i >= 0; i--) {
        s32 randIndex = random_float() * (i + 1);
        struct Object *tmp = starList[randIndex];
        starList[randIndex] = starList[i];
        starList[i] = tmp;
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

#ifdef SET_HOMES
        o->oHomeX = approach_f32_asymptotic(o->oHomeX, posStore[i][0], FACTOR);
        o->oHomeY = approach_f32_asymptotic(o->oHomeY, posStore[i][1], FACTOR);
        o->oHomeZ = approach_f32_asymptotic(o->oHomeZ, posStore[i][2], FACTOR);
#endif // SET_HOMES

        o->header.gfx.pos[0] = approach_f32_asymptotic(o->header.gfx.pos[0], posStore[i][0], FACTOR);
        o->header.gfx.pos[1] = approach_f32_asymptotic(o->header.gfx.pos[1], posStore[i][1], FACTOR);
        o->header.gfx.pos[2] = approach_f32_asymptotic(o->header.gfx.pos[2], posStore[i][2], FACTOR);

        // make object visible if close to mario now
        if (o->oFlags & OBJ_FLAG_COMPUTE_DIST_TO_MARIO) {
            o->oDistanceToMario = dist_between_objects(o, gMarioObject);
            if (o->oDistanceToMario < o->oDrawingDistance) {
                gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
                gCurrentObject->activeFlags &= ~ACTIVE_FLAG_FAR_AWAY;
            }
        }
    }
}

void cshuffle_finish_approach(UNUSED struct Camera *c) {
    for (u32 i = 0; i < shuffleCount; i++) {
        struct Object *o = shuffleList[i];
        if (gMarioObject == o) continue;

        o->oPosX = posStore[i][0];
        o->oPosY = posStore[i][1];
        o->oPosZ = posStore[i][2];

#ifdef SET_HOMES
        o->oHomeX = posStore[i][0];
        o->oHomeY = posStore[i][1];
        o->oHomeZ = posStore[i][2];
#endif // SET_HOMES

        o->header.gfx.pos[0] = posStore[i][0];
        o->header.gfx.pos[1] = posStore[i][1];
        o->header.gfx.pos[2] = posStore[i][2];
    }

    for (u32 i = 0; i < starCount; i++) {
        struct Object *o = starList[i];

        o->oPosX = starPos[i][0];
        o->oPosY = starPos[i][1];
        o->oPosZ = starPos[i][2];

#ifdef SET_HOMES
        o->oHomeX = posStore[i][0];
        o->oHomeY = posStore[i][1];
        o->oHomeZ = posStore[i][2];
#endif // SET_HOMES

        o->header.gfx.pos[0] = starPos[i][0];
        o->header.gfx.pos[1] = starPos[i][1];
        o->header.gfx.pos[2] = starPos[i][2];
    }
}

void cshuffle_setfloors_androoms(UNUSED struct Camera *c) {
    for (int ol = 1; ol < NUM_OBJ_LISTS; ol++) {
        struct Object *head = (struct Object *)&gObjectLists[ol];
        if (head == NULL) continue;

        struct Object *o = head;
        do {
            if (o->oMoveFlags & OBJ_DROPPED_TO_FLOOR) {
                f32 floor = find_floor_height(o->oPosX, o->oPosY + 200.0f, o->oPosZ);
                o->oPosY = floor;
                o->oHomeY = floor;
                o->header.gfx.pos[1] = floor;
            }

            if ((o->parentObj != o)
             && isBeingShuffled(o->parentObj)
             && (IS_INLIST(o, surflist))
             && (!IS_INLIST(o, ignorelist))
            ) {
                vec3f_copy(&o->oPosX, &o->parentObj->oPosX);
#ifdef SET_HOMES
                vec3f_copy(&o->oHomeX, &o->parentObj->oHomeX);
#endif // SET_HOMES
                vec3f_copy(o->header.gfx.pos, o->parentObj->header.gfx.pos);
            }

            if (o->oRoom != -1) {
                struct Object *prev = gCurrentObject;
                gCurrentObject = o;
                void bhv_init_room(); bhv_init_room();
                gCurrentObject = prev;
            }

            o = (struct Object*) o->header.next;
        } while (o != head);
    }
}

void cshuffle_timestop(UNUSED struct Camera *c) {
    enable_time_stop_including_mario();
}
void cshuffle_timestart(UNUSED struct Camera *c) {
    disable_time_stop_including_mario();
    gWDWWaterLevelChanging = FALSE;
}

extern void cutscene_goto_cvar_pos(struct Camera *c, f32 goalDist, s16 goalPitch, s16 rotPitch, s16 rotYaw);
extern s16 sStatusFlags;
void cutscene_shuffle(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    cutscene_event(cshuffle_reset, c, 0, 1);
    cutscene_event(cshuffle_timestop, c, 0, 1);
    cutscene_event(cshuffle_populate_shuffle_list, c, 0, 1);
    cutscene_event(cshuffle_populate_star_list, c, 0, 1);
    cutscene_event(cshuffle_populate_surface_list, c, 1, 2);
    cutscene_event(cshuffle_sort, c, 1, 2);
    cutscene_event(cshuffle_approach, c, 30, 59);
    cutscene_event(cshuffle_finish_approach, c, 59, -1);
    cutscene_event(cshuffle_setfloors_androoms, c, 59, -1);
    cutscene_event(cshuffle_timestart, c, 59, -1);

    f32 floor = gMarioState->floorHeight;
    c->pos[1] = floor + 50.0f;

    void print_text_centered(int, int, char*);
    print_text_centered(160, 10, "SHUFFLING OBJECTS");
}

void cutscene_shuffle_end(struct Camera *c) {
    c->cutscene = 0;
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
}

struct Cutscene sCutsceneShuffleObjs[] = {
    {cutscene_shuffle, 60},
    {cutscene_shuffle_end, 0},
};

u32 chsStartShuffle = 0;

void chs_start_shuffle(void) {
    chsStartShuffle = 1;
}

#define GROUNDCOND (gMarioState->action & (ACT_FLAG_STATIONARY | ACT_FLAG_MOVING))
#define SWIMCOND (gMarioState->action & ACT_FLAG_SWIMMING)
#define SHELLCOND (gMarioState->action == ACT_RIDING_SHELL_GROUND)
#define WINGCOND (gMarioState->action & ACT_FLAG_SWIMMING_OR_FLYING)

void chs_shuffle_objects(void) {
    if (chsStartShuffle == 1) {
        if (GROUNDCOND || SWIMCOND || SHELLCOND || WINGCOND) {
            if (gCurrCourseNum != COURSE_NONE) {
                gMarioState->statusForCamera->cameraEvent = CAM_EVENT_SHUFFLE;
            }

            chsStartShuffle = 0;
        }
    }
}
