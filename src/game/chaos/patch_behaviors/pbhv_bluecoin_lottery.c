#include "actors/group0.h"
#include "behavior_data.h"
#include "object_fields.h"
#include "sounds.h"
#include "audio/external.h"
#include "game/ingame_menu.h"
#include "game/game_init.h"
#include "game/camera.h"
#include "game/debug.h"
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
#define SUPERSTAR_INDEX 1

enum SlotStates {
    S_STANDBY = 0,
    S_GO,
    S_SHOWUP,
    S_ROLL,
    S_STOP,
    S_SHOWDOWN,
    S_FINISH
};

// to get slot i: 30 + (60 * i)
// i = 1 for blue coin
u32 slot_state = S_STANDBY;
u32 slot_nextstate = S_STANDBY;
u32 slot_timer = 0;
struct Object *currCoin = NULL;

static s32 shouldWinSlots = FALSE;
#define WIN_CHANCE 0.05f

#define OFFSCREEN_POS -50.0f
static f32 globalY = OFFSCREEN_POS;
u32 timers[NUM_SLOTS] = {16, 24, 32};
u32 speeds[NUM_SLOTS];
u16 rotations[NUM_SLOTS];
s32 pregenerated[NUM_SLOTS];

u32 interact_coin_delayed(struct MarioState *m, struct Object *obj) {
    s32 coinCount = obj->oDamageOrCoinValue;
    s32 healCount = 0;

    chs_collectors_anxiety_coin_collected(coinCount);
    if (chs_double_coins_under_30s()) {
        coinCount *= 2;
    }

    m->numCoins += coinCount;

    if (!chaos_check_if_patch_active(CHAOS_PATCH_NOHEAL_COINS)) {
        if (!(obj_has_behavior(obj, bhvYellowCoin) && obj->oDroppedCoinBounce)) {
            u8 healingMult = 4;
            if (chaos_check_if_patch_active(CHAOS_PATCH_HEALING_BONUS)) {
                healingMult *= 2;
            }
            healCount += (healingMult * coinCount);
        }
    }

    m->healCounter += healCount;
    if (healCount > m->healCounter) {
        m->healCounter = U8_MAX;
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

void reset_slots(void) {
    if (slot_state != S_STANDBY) {
        disable_time_stop_including_mario();
        if (gPauseDisabled > 0) {
            gPauseDisabled--;
        }
    }

    currCoin = NULL;
    slot_state = S_STANDBY;
    slot_nextstate = S_STANDBY;
    slot_timer = 0;
}

void init_slots(struct Object *oo, f32 chance) {
    globalY = OFFSCREEN_POS;
    currCoin = oo;
    shouldWinSlots = (chance < WIN_CHANCE);

    if (!currCoin) {
        return;
    }

    if (!COURSE_IS_MAIN_COURSE(gCurrCourseNum)) {
        shouldWinSlots = FALSE;
    }

    slot_nextstate = S_GO;
    for (int i = 0; i < NUM_SLOTS; i++) {
        rotations[i] = 30;
        speeds[i] = (random_u16() % 12) + 26;
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
    s32 win;
    s32 winningNumber;

    if (sCurrPlayMode != PLAY_MODE_NORMAL) {
        return;
    }

    switch (slot_state) {
        case S_STANDBY: break;
        case S_GO:
            gPauseDisabled++;
            enable_time_stop_including_mario();
            slot_nextstate = S_SHOWUP;
            break;
        case S_SHOWUP:
            if (slot_timer == 0) {
                play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
            }
            globalY = approach_f32_asymptotic(globalY, 30.0f, 0.25f);
            if (slot_timer > 10) {
                slot_nextstate = S_ROLL;
            }
            break;
        case S_ROLL:
            if (slot_timer == 0) {
                win = TRUE;

                for (int i = 0; i < NUM_SLOTS; i++) {
                    s32 generatedNumber = (random_u16() % 6); // Generate landing number at random
                    if (i == 0) {
                        winningNumber = generatedNumber;
                    } else if (generatedNumber != winningNumber) {
                        win = FALSE;
                    }

                    if (shouldWinSlots) {
                        generatedNumber = winningNumber;
                    } else {
                        // Rig the slots for guaranteed loss
                        if (win && i == NUM_SLOTS - 1) {
                            generatedNumber = (generatedNumber + (random_u16() % (6 - 1)) + 1) % 6;
                        }
                    }

                    pregenerated[i] = generatedNumber;
                }

                // The SM64DS slot minigames treats the superstar as a wild. This could make things confusing for the player.
                // Instead, make sure superstar shows up at most once. Do not allow 2+ superstars or 1 superstar + 2 matching others.
                if (!shouldWinSlots) {
                    s32 doubleMatch = FALSE;
                    s32 sameItemType = -1;

                    for (int i = 0; i < NUM_SLOTS; i++) {
                        // At most one superstar may show here
                        if (pregenerated[i] == SUPERSTAR_INDEX) {
                            if (doubleMatch) {
                                pregenerated[i] = (pregenerated[i] + (random_u16() % (6 - 1)) + 1) % 6;
                                assert(pregenerated[i] != SUPERSTAR_INDEX, "drawslots:\nGenerated additional superstar when should not have been possible!");
                            } else {
                                doubleMatch = TRUE;
                                continue;
                            }
                        }

                        if (sameItemType < 0) {
                            sameItemType = pregenerated[i];
                            continue;
                        }

                        if (sameItemType != pregenerated[i]) {
                            // Couldn't have possibly won, carry on
                            break;
                        }

                        if (doubleMatch) {
                            s32 superstarDiff = ((SUPERSTAR_INDEX + 6) - pregenerated[i]) % 6;
                            s32 newIndexDiff = (random_u16() % (6 - 2)) + 1;
                            if (newIndexDiff >= superstarDiff) {
                                newIndexDiff++;
                            }

                            pregenerated[i] = (pregenerated[i] + newIndexDiff) % 6;

                            assert(pregenerated[i] != SUPERSTAR_INDEX, "drawslots:\nGenerated additional superstar when should not have been possible!");
                            assert(pregenerated[i] != sameItemType, "drawslots:\nsameItemType should not have been duplicateable here!");
                            break;
                        } else {
                            doubleMatch = TRUE;
                        }
                    }
                }

                for (int i = 0; i < NUM_SLOTS; i++) {
                    // Precalculation to determine exactly where final slots will land
                    rotations[i] = 360 - ((speeds[i] * timers[i]) % 360);
                    rotations[i] = (rotations[i] + (pregenerated[i] * 60) + 30) % 360;
                }
            }

            for (int i = 0; i < NUM_SLOTS; i++) {
                if (slot_timer < timers[i]) {
                    rotations[i] = (rotations[i] + speeds[i]) % 360;
                    if (slot_timer == (timers[i] - 1)) {
                        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
                    }
                } else if (i == NUM_SLOTS - 1) {
                    slot_nextstate = S_STOP;
                }
            }

            break;
        case S_STOP:
            // Determine if slots were won (in the unlikely case rigging logic is somehow broken)
            win = TRUE;
            winningNumber = (rotations[0] / 60) % 6;
            for (int i = 1; i < NUM_SLOTS; i++) {
                if (winningNumber != (rotations[i] / 60) % 6) {
                    win = FALSE;
                    break;
                }
            }

            if (win) {
                currCoin->oDamageOrCoinValue = 100;
                if (slot_timer == 0) {
                    play_sound(SOUND_GENERAL2_RIGHT_ANSWER, gGlobalSoundSource);
                }
            } else {
                currCoin->oDamageOrCoinValue = 5;
                if (slot_timer == 0) {
                    play_sound(SOUND_GENERAL2_CAMERA_BUZZ, gGlobalSoundSource);
                }
            }

            if (slot_timer > 10) {
                slot_nextstate = S_SHOWDOWN;
            }
            break;
        case S_SHOWDOWN:
            if (slot_timer == 0) {
                play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
            }
            globalY = approach_f32_asymptotic(globalY, OFFSCREEN_POS, 0.25f);
            if (slot_timer > 10) {
                slot_nextstate = S_FINISH;
            }
            break;
        case S_FINISH:
            disable_time_stop_including_mario();
            if (gPauseDisabled > 0) {
                gPauseDisabled--;
            }
            slot_nextstate = S_STANDBY;
            interact_coin_delayed(gMarioState, currCoin);
            // Deactivate patch
            if (currCoin->oDamageOrCoinValue == 100) {
                for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
                    if (gChaosActiveEntries[i].id != CHAOS_PATCH_BLUECOIN_LOTTERY) {
                        continue;
                    }

                    chaos_remove_expired_entry(i--, "%s: Expired!"); // NOTE: Not inside callback, doesn't need to be deferred
                    break;
                }
            }

            obj_mark_for_deletion(currCoin);
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
