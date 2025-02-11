#include "actors/group0.h"
#include "behavior_data.h"
#include "object_fields.h"
#include "sounds.h"
#include "audio/external.h"
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

static f32 chanceroll = 0;
#define CHANCE 0.03f

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

u32 interact_coin_delayed(struct MarioState *m, struct Object *obj) {
    s32 coinCount = obj->oDamageOrCoinValue;
    s32 healCount = 0;
    if (chs_double_coins_under_30s()) {
        coinCount *= 2;
    }

    m->numCoins += coinCount;

    if (!chaos_check_if_patch_active(CHAOS_PATCH_NOHEAL_COINS)) {
        if (!(obj_has_behavior(obj, bhvYellowCoin) && obj->oDroppedCoinBounce)) {
            healCount += 4 * coinCount;
        }
    }

    m->healCounter += healCount;
    if (healCount > m->healCounter) {
        healCount = U8_MAX;
    }

    obj->oInteractStatus = INT_STATUS_INTERACTED;

    if (COURSE_IS_MAIN_COURSE(gCurrCourseNum) && m->numCoins - coinCount < (100 + m->hundredCoinOffset)
        && m->numCoins >= (100 + m->hundredCoinOffset)) {
        struct Object *tmp = gCurrentObject;
        gCurrentObject = obj;
        bhv_spawn_star_no_level_exit(6);
        gCurrentObject = tmp;
    }
#if ENABLE_RUMBLE
    if (obj->oDamageOrCoinValue >= 2) {
        queue_rumble_data(5, 80);
    }
#endif

    return FALSE;
}

void init_slots(struct Object *oo, f32 chance) {
    globalY = OFFSCREEN_POS;
    currCoin = oo;
    chanceroll = chance;

    if (!COURSE_IS_MAIN_COURSE(gCurrCourseNum)) {
        chanceroll = 1;
    }

    slot_nextstate = S_GO;
    for (int i = 0; i < NUM_SLOTS; i++) {
        rotations[i] = 0;
    }
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
                if (slot_timer == 0) {
                    play_sound(SOUND_GENERAL2_RIGHT_ANSWER, gGlobalSoundSource);
                }
            } else {
                currCoin->oDamageOrCoinValue = 5;
                if (slot_timer == 0) {
                    play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
                }
            }

            if (slot_timer > 10) {
                slot_nextstate = S_SHOWDOWN;
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
            slot_nextstate = S_STANDBY;
            interact_coin_delayed(gMarioState, currCoin);
            // Deactivate patch
            if (currCoin->oDamageOrCoinValue == 100) {
                for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
                    if (gChaosActiveEntries[i].id != CHAOS_PATCH_BLUECOIN_LOTTERY) {
                        continue;
                    }

                    chaos_remove_expired_entry(i, "%s: Expired!");
                    break;
                }
            }

            currCoin = NULL;
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
