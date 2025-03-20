#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "course_table.h"
#include "sm64.h"
#include "game/area.h"
#include "game/level_update.h"
#include "game/mario.h"
#include "game/print.h"
#include "dialog_ids.h"
#include "game/ingame_menu.h"
#include "game/game_init.h"
#include "behavior_data.h"
#include "game/object_helpers.h"
#include "audio/external.h"
#include "game/chaos/chaos.h"

#define SLEEP_TIME_MIN  (30 * 30)
#define SLEEP_TIME_RAND (5 * 60 * 30)

#define SHOCK_TIME_MIN  (15 * 30)
#define SHOCK_TIME_RAND (2 * 60 * 30)

#define BURN_TIME_MIN  (30 * 30)
#define BURN_TIME_RAND (240 * 30)

s16 sRandomSleepTimer = -1;
s16 sRandomShockTimer = -1;
s16 sRandomBurnTimer = -1;

/*
    Sleeping
*/
void chs_act_random_sleep(void) {
    sRandomSleepTimer = RAND(SLEEP_TIME_RAND) + SLEEP_TIME_MIN;
}

void chs_update_random_sleep(void) {
    if(sRandomSleepTimer == 0) {
        s32 actGroup = (gMarioState->action & ACT_GROUP_MASK);
        if(actGroup == ACT_GROUP_MOVING || actGroup == ACT_GROUP_STATIONARY) {
            set_mario_action(gMarioState, ACT_START_SLEEPING, 0);
            sRandomSleepTimer = -1;
        }
    } else if (sRandomSleepTimer == -1) {
        if(gMarioState->action != ACT_START_SLEEPING) {
            sRandomSleepTimer = RAND(SLEEP_TIME_RAND) + SLEEP_TIME_MIN;
        }
    } else {
        sRandomSleepTimer--;
    }
}

/*
    Shocking
*/

void chs_act_random_shock(void) {
    sRandomShockTimer = RAND(SHOCK_TIME_RAND) + SHOCK_TIME_MIN;
}

void chs_update_random_shock(void) {
    if(sRandomShockTimer == 0) {
        sRandomShockTimer = -1;
    } else if (sRandomShockTimer == -1) {
        s32 actGroup = (gMarioState->action & ACT_GROUP_MASK);
        if(!(actGroup == ACT_GROUP_CUTSCENE || actGroup == ACT_GROUP_AUTOMATIC)) {
            hurt_and_set_mario_action(gMarioState, ACT_SHOCKED, 0, 4);
            sRandomShockTimer = RAND(SHOCK_TIME_RAND) + SHOCK_TIME_MIN;
        }
    } else {
        sRandomShockTimer--;
    }
}

u8 chs_cond_random_shock(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_ONE_HIT_WONDER));
}

/*
    Burning
*/

void chs_act_random_burn(void) {
    sRandomBurnTimer = RAND(BURN_TIME_RAND) + BURN_TIME_MIN;
}

void chs_update_random_burn(void) {
    if (sRandomBurnTimer == 0) {
        sRandomBurnTimer = -1;
    } else if (sRandomBurnTimer == -1) {
        s32 actGroup = (gMarioState->action & ACT_GROUP_MASK);
        if(!(actGroup == ACT_GROUP_CUTSCENE || actGroup == ACT_GROUP_SUBMERGED || actGroup == ACT_GROUP_AUTOMATIC)) {
            gMarioState->marioObj->oMarioBurnTimer = 0;
            u32 burningAction = ACT_BURNING_JUMP;

            play_mario_sound(gMarioState, SOUND_MARIO_ON_FIRE, 0);
            if (!chs_check_temporary_invincibility()) {
                if (chaos_check_if_patch_active(CHAOS_PATCH_SONIC_SIMULATOR) && gCurrCourseNum != COURSE_NONE) {
                    set_hurt_counter(gMarioState, (gMarioState->flags & MARIO_CAP_ON_HEAD) ? 12 : 18);
                }
            }

            if ((gMarioState->action & ACT_FLAG_AIR) && gMarioState->vel[1] <= 0.0f) {
                burningAction = ACT_BURNING_FALL;
            }

            set_mario_action(gMarioState, burningAction, 0);

            sRandomBurnTimer = RAND(BURN_TIME_RAND) + BURN_TIME_MIN;
        }
    } else {
        sRandomBurnTimer--;
    }
}

u8 chs_cond_random_burn(void) {
    return(!chaos_check_if_patch_active(CHAOS_PATCH_ONE_HIT_WONDER));
}

/*
    Blinding
*/

#define BLIND_TIME_MAX      18000
#define BLIND_TIME_END      18300

void chs_act_random_blind(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_RANDOM_BLIND, &this);
    this->frameTimer = RAND(BLIND_TIME_MAX); //Get a random offset to start the timer at
}

void chs_update_random_blind(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_RANDOM_BLIND, &this);
    if(this->frameTimer > BLIND_TIME_END) {
        this->frameTimer = RAND(BLIND_TIME_MAX); //Get a random offset to restart the timer at
    }
}

/*
    Dialogue
*/

#define DIALOGUE_TIME_MAX      (4 * 60 * 30)

void chs_act_random_dialogue(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_RANDOM_DIALOGUE, &this);
    this->frameTimer = RAND(DIALOGUE_TIME_MAX); //Get a random offset to start the timer at
}

void chs_update_random_dialogue(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_RANDOM_DIALOGUE, &this);
    if(this->frameTimer > DIALOGUE_TIME_MAX && !(gMarioState->action & ACT_GROUP_CUTSCENE)) {
        create_dialog_box(RAND(DIALOG_COUNT));
        gChsTrollDialog = TRUE;
        this->frameTimer = RAND(DIALOGUE_TIME_MAX); //Get a random offset to restart the timer at
    }
}

/*
    Kaizo Blocks
*/

#define KAIZO_BLOCK_TIME_MAX    (4 * 60 * 30)

void chs_act_kaizo_blocks(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_KAIZO_BLOCKS, &this);
    this->frameTimer = RAND(KAIZO_BLOCK_TIME_MAX); //Get a random offset to start the timer at
}

void chs_update_kaizo_blocks(void) {
    struct MarioState *m = gMarioState;
    f32 heightAboveFloor = m->pos[1] - m->floorHeight;
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_KAIZO_BLOCKS, &this);

    if(this->frameTimer > KAIZO_BLOCK_TIME_MAX && m->vel[1] > 15.0f && heightAboveFloor > 120.0f) {
        spawn_object_abs_with_rot(m->marioObj, 0, MODEL_KAIZO_BLOCK, bhvKaizoBlock,
                            m->pos[0], m->pos[1] + 170.0f, m->pos[2], 0, 0, 0);
        m->vel[1] = -20.0f;
        if (m->marioObj) {
            m->marioObj->oVelY = m->vel[1];
        }
        this->frameTimer = RAND(KAIZO_BLOCK_TIME_MAX); //Get a random offset to start the timer at
    }
}

/*
    Troll Sounds
*/

#define TROLL_SOUNDS_TIME_MAX   18000

void chs_act_troll_sounds(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_TROLL_SOUNDS, &this);
    this->frameTimer = RAND(TROLL_SOUNDS_TIME_MAX); //Get a random offset to start the timer at
}

void chs_update_troll_sounds(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_TROLL_SOUNDS, &this);
    
    if(this->frameTimer > TROLL_SOUNDS_TIME_MAX) {
        s32 rand = RAND(100);

        if(rand > 90) {
            //5% chance
            rand = RAND(2);
            if(rand) {
                play_sound(SOUND_MENU_TROLL_TROMBONE, gGlobalSoundSource);
            } else {
                play_sound(SOUND_MENU_TROLL_SKYPE, gGlobalSoundSource);
            }
        } else if (rand > 50) {
            //13.333...% chance
            rand = RAND(3);
            switch(rand) {
                case 0:
                    play_sound(SOUND_MENU_TROLL_KNOCK, gGlobalSoundSource);
                    break;
                case 1:
                    play_sound(SOUND_MENU_TROLL_NOTIF, gGlobalSoundSource);
                    break;
                case 2:
                    play_sound(SOUND_MENU_TROLL_JOIN, gGlobalSoundSource);
                    break;
            }
        } else {
            //25% chance
            rand = RAND(2);
            switch(rand) {
                case 0:
                    play_sound(SOUND_MENU_TROLL_PING, gGlobalSoundSource);
                    break;
                case 1:
                    play_sound(SOUND_MENU_TROLL_USB, gGlobalSoundSource);
                    break;
            }
        }

        this->frameTimer = RAND(TROLL_SOUNDS_TIME_MAX); //Get a random offset to start the timer at
    }
}

/*
    Red Light Green Light
*/

#define RED_LIGHT_TIME_MAX      (7 * 30)
#define RED_LIGHT_TIME_MIN      (2 * 30)
#define RED_LIGHT_RAND          (RED_LIGHT_TIME_MAX - RED_LIGHT_TIME_MIN)

#define GREEN_LIGHT_TIME_MAX    (3 * 60 * 30)
#define GREEN_LIGHT_TIME_MIN    (45 * 30)
#define GREEN_LIGHT_RAND        (GREEN_LIGHT_TIME_MAX - GREEN_LIGHT_TIME_MIN)

#define CHS_GREEN_LIGHT         0
#define CHS_RED_LIGHT           1

u8 sChsRedLightPhase = CHS_GREEN_LIGHT;
s16 sChsRedLightTextTimer = 0;
u8 sChsRedLightCounter = 0;

void chs_act_red_light(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_RED_LIGHT, &this);
    this->frameTimer = RAND(GREEN_LIGHT_RAND);
    sChsRedLightPhase = CHS_GREEN_LIGHT;
    sChsRedLightTextTimer = 30;
    sChsRedLightCounter = 0;
}

void chs_update_red_light(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_RED_LIGHT, &this);
    if(sChsRedLightPhase == CHS_GREEN_LIGHT) {
        if(sChsRedLightTextTimer++ < 30) {
            print_text_centered(SCREEN_CENTER_X, 40, "GREEN LIGHT");
        }

        s16 timeLeft = GREEN_LIGHT_TIME_MAX - this->frameTimer;

        switch(timeLeft) {
            case 90:
                play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
                sChsRedLightCounter = 3;
                break;
            case 60:
                play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
                sChsRedLightCounter = 2;
                break;
            case 30:
                play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
                sChsRedLightCounter = 1;
                break;
        }

        if(sChsRedLightCounter) {
            print_text_fmt_int(SCREEN_CENTER_X, 40, "%d", sChsRedLightCounter);
        }

        if(this->frameTimer > GREEN_LIGHT_TIME_MAX) {
            this->frameTimer = RAND(RED_LIGHT_RAND);
            sChsRedLightPhase = CHS_RED_LIGHT;
            play_sound(SOUND_MENU_PAUSE_OPEN, gGlobalSoundSource);
            sChsRedLightCounter = 0;
        }
    } else {
        if(this->frameTimer > RED_LIGHT_TIME_MAX) {
            this->frameTimer = RAND(GREEN_LIGHT_RAND);
            sChsRedLightPhase = CHS_GREEN_LIGHT;
            sChsRedLightTextTimer = 0;
            play_sound(SOUND_MENU_PAUSE_CLOSE, gGlobalSoundSource);
            sChsRedLightCounter = 0;
        } else {
            print_text_centered(SCREEN_CENTER_X, 40, "RED LIGHT");

            if(gPlayer1Controller->buttonPressed || gPlayer1Controller->buttonDown || gPlayer1Controller->stickMag) {
                gMarioState->health = 0;
            }
        }
    }
}

u8 chs_cond_red_light(void) {
    return (!(chaos_check_if_patch_active(CHAOS_PATCH_GREEN_DEMON) || chaos_check_if_patch_active(CHAOS_PATCH_COSMIC_CLONES)));
}

/*
    Cosmic Rays
*/

#define COSMIC_RAYS_TIME_MAX    (3 * 60 * 30)

void chs_act_cosmic_rays(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_COSMIC_RAYS, &this);
    this->frameTimer = RAND(COSMIC_RAYS_TIME_MAX);
}

void chs_update_cosmic_rays(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_COSMIC_RAYS, &this);

    if(this->frameTimer > COSMIC_RAYS_TIME_MAX) {
        gMarioState->pos[1] += 3155.25f;
        gMarioState->peakHeight = gMarioState->pos[1];
        this->frameTimer = RAND(COSMIC_RAYS_TIME_MAX);
    }
}
