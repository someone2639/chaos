#include "actors/group0.h"
#include "object_fields.h"
#include "game/ingame_menu.h"
#include "game/game_init.h"
#include "game/camera.h"
#include "game/object_helpers.h"
#include "game/level_update.h"
#include "game/object_list_processor.h"
#include "engine/behavior_script.h"
#include "game/patch_selection_ui.h"
#include "game/interaction.h"
#include "game/behavior_actions.h"
#include "game/rumble_init.h"
#include "course_table.h"

#define NUM_SLOTS 3

// to get slot i: 30 + (60 * i)
// i = 1 for blue coin
u32 slot_state = 0;
u32 slot_nextstate = 0;
u32 slot_timer = 0;
struct Object *currCoin = NULL;
u32 slot_semaphore = 0;

static f32 chanceroll = 0;
#define CHANCE 0.01f

#define OFFSCREEN_POS -50.0f
static f32 globalY = OFFSCREEN_POS;
u32 timers[NUM_SLOTS] = {15, 23, 31};
u16 rotations[NUM_SLOTS];

enum SlotStates {
    S_STANDBY = 0,
    S_GO,
    S_SHOWUP,
    S_ROLL,
    S_STOP,
    S_SHOWDOWN,
    S_FINISH
};

u32 interact_coin_delayed(struct Object *o, struct MarioState *m) {
    m->numCoins += o->oDamageOrCoinValue;
    m->healCounter += 4 * o->oDamageOrCoinValue;

    o->oInteractStatus = INT_STATUS_INTERACTED;

    if (o->oDamageOrCoinValue == 100) {
        bhv_spawn_star_no_level_exit(6);
    }
#if ENABLE_RUMBLE
    if (o->oDamageOrCoinValue >= 2) {
        queue_rumble_data(5, 80);
    }
#endif

    return FALSE;
}

void init_slots(struct Object *oo, f32 chance) {
    globalY = OFFSCREEN_POS;
    currCoin = oo;
    chanceroll = chance;

    if (gCurrCourseNum == COURSE_PSS) {
        chanceroll = 1;
    }

    slot_nextstate = S_GO;
    for (int i = 0; i < NUM_SLOTS; i++) {
        rotations[i] = 0;
    }
    slot_semaphore = 1;
}

// (60 * i) + (gGlobalTimer * 16)

void slot_draw(int timer, int x, int y) {
    Mtx trans;
    Mtx rot;
    Mtx scale;
    Mtx SR;
    Mtx *final = alloc_display_list(sizeof(Mtx));

    if (final == NULL) {
        return;
    }

    guScale(&scale, 0.25f, 0.25f, 0.25f);
    guRotate(&rot, timer, 1, 0, 0);
    guTranslate(&trans, x, y, -40);
    guMtxCatL(&scale, &rot, &SR);
    guMtxCatL(&SR, &trans, final);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(final++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF,G_RM_OPA_SURF2);
    gSPDisplayList(gDisplayListHead++, &slotwheel_slotwheel_mesh_layer_1);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void drawslots() {
    switch (slot_state) {
        case S_STANDBY: break;
        case S_GO:
            enable_time_stop_including_mario();
            slot_nextstate = S_SHOWUP;
            break;
        case S_SHOWUP:
            globalY = approach_f32_asymptotic(globalY, 30.0f, 0.25f);
            if (slot_timer > 10) {
                slot_nextstate = S_ROLL;
            }
            break;
        case S_ROLL:
            for (int i = 0; i < NUM_SLOTS; i++) {
                if (slot_timer > timers[i]) {
                    if (chanceroll < CHANCE) {
                        rotations[i] = 90;
                    } else {
                        if (((rotations[i] + 30) % 60) > 0) {
                            rotations[i] -= ((rotations[i] + 30) % 60);
                        } 
                    }
                    if (i == NUM_SLOTS - 1) {
                        slot_nextstate = S_STOP;
                    }
                } else {
                    rotations[i] = (60 * i) + (gGlobalTimer * 32);
                }
            }
            break;
        case S_STOP:
            if (rotations[NUM_SLOTS - 1] == 90) {
                currCoin->oDamageOrCoinValue = 100;
                slot_semaphore = 0;
                // TODO: success sound
                interact_coin_delayed(currCoin, gMarioState);
                slot_nextstate = S_SHOWDOWN;
            } else {
                currCoin->oDamageOrCoinValue = 5;
                // TODO: failure sound
                if (slot_timer > 10) {
                    slot_nextstate = S_SHOWDOWN;
                }
            }
            break;
        case S_SHOWDOWN:
            globalY = approach_f32_asymptotic(globalY, OFFSCREEN_POS, 0.25f);
            if (slot_timer > 10) {
                slot_nextstate = S_FINISH;
            }
            break;
        case S_FINISH:
            disable_time_stop_including_mario();
            slot_semaphore = 0;
            currCoin = NULL;
            slot_nextstate = S_STANDBY;
            break;
    }

    if (slot_state != S_STANDBY) {
        for (int i = 0; i < NUM_SLOTS; i++) {
            slot_draw(rotations[i], 30 + (60 * i), globalY);
        }
    }

    slot_timer++;
    if (slot_nextstate != slot_state) {
        slot_state = slot_nextstate;
        slot_timer = 0;
    }
}
