#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/chaos/chaos.h"
#include "game/chaos_menus.h"
#include "engine/math_util.h"
#include "game/game_init.h"
#include "game/object_list_processor.h"
#include "game/level_update.h"
#include "sm64.h"
#include "game/ingame_menu.h"
#include "audio/external.h"

#define MAX_PROMPTS                 8
#define MIN_PROMPTS                 3
#define NUM_QTE                     9
#define QTE_PADDING                 18
#define QTE_ACTIVATE_TIME           5400
#define QTE_INPUT_TIME_PER_INPUT    22

struct QuickTimePrompt {
    u16 button;
    enum MenuButtonPrompt prompt;
};

u32 sQTEButtonLUT[NUM_QTE * 2] = {
    A_BUTTON,       MENU_PROMPT_A_BUTTON,
    B_BUTTON,       MENU_PROMPT_B_BUTTON,
    R_TRIG,         MENU_PROMPT_R_TRIG,
    Z_TRIG,         MENU_PROMPT_Z_TRIG,
    START_BUTTON,   MENU_PROMPT_START_BUTTON,
    U_CBUTTONS,     MENU_PROMPT_C_UP,
    L_CBUTTONS,     MENU_PROMPT_C_LEFT,
    R_CBUTTONS,     MENU_PROMPT_C_RIGHT,
    D_CBUTTONS,     MENU_PROMPT_C_DOWN,
};

struct QuickTimePrompt sQTEPromptQueue[MAX_PROMPTS];
u8 sQTEQueueLength = MIN_PROMPTS;
u8 sQTECurrentIndex = 0;
s16 sQTETimeLeft = 0;
s16 sQTETimeTotal = QTE_INPUT_TIME_PER_INPUT;

void generate_qte() {
    sQTECurrentIndex = 0;
    sQTEQueueLength = MIN_PROMPTS + RAND((MAX_PROMPTS - MIN_PROMPTS + 1));
    sQTETimeTotal = QTE_INPUT_TIME_PER_INPUT * sQTEQueueLength;
    sQTETimeLeft = sQTETimeTotal;

    s32 qte = 0;
    for(int i = 0; i < sQTEQueueLength; i++) {
        qte = RAND(NUM_QTE);
        sQTEPromptQueue[i].button = sQTEButtonLUT[(qte * 2)];
        sQTEPromptQueue[i].prompt = sQTEButtonLUT[(qte * 2) + 1];
    }

    set_play_mode(PLAY_MODE_QUICKTIME);
}

void draw_quicktime_event_prompts() {
    if(sQTETimeLeft < 1) {
        return;
    }

    create_dl_ortho_matrix(&gDisplayListHead);
    
    shade_screen();

    menu_start_button_prompt();
    f32 startX = SCREEN_CENTER_X - ((sQTEQueueLength * QTE_PADDING) / 2);
    f32 timerBarWidth = (((SCREEN_WIDTH - startX) - startX) / sQTETimeTotal) * sQTETimeLeft;

    for(int i = sQTECurrentIndex; i < sQTEQueueLength; i++) {
        menu_button_prompt(startX + (QTE_PADDING * i), SCREEN_CENTER_Y, sQTEPromptQueue[i].prompt);
    }

    menu_end_button_prompt();

    gDPPipeSync(gDisplayListHead++);
    gDPSetCombineLERP(gDisplayListHead++, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT);
    gDPSetFillColor(gDisplayListHead++, (GPACK_RGBA5551(255, 255, 255, 1) << 16) | GPACK_RGBA5551(255, 255, 255, 1));
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    gDPFillRectangle(gDisplayListHead++, startX, (SCREEN_CENTER_Y + QTE_PADDING), startX + timerBarWidth, (SCREEN_CENTER_Y + QTE_PADDING + 2));
    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
}

void play_mode_quicktime() {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_QUICKTIME, &this);

    if(sQTETimeLeft > 0) {
        u16 buttons = gPlayer1Controller->buttonPressed;
        if(buttons) {
            if(buttons == sQTEPromptQueue[sQTECurrentIndex].button) { // NO BUTTON MASHING !!!!
                sQTECurrentIndex++;
                play_sound(SOUND_MENU_POWER_METER, gGlobalSoundSource);
            } else {
                if(sQTETimeLeft < sQTETimeTotal - 20) {
                    sQTETimeLeft -= QTE_INPUT_TIME_PER_INPUT;
                }
                play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
            }
        }

        if(sQTECurrentIndex == sQTEQueueLength) {
            sQTETimeLeft = -1;
            this->frameTimer = RAND(QTE_ACTIVATE_TIME);
            set_play_mode(PLAY_MODE_NORMAL);
        } else {
            sQTETimeLeft--;
        }

        if(sQTETimeLeft < 0) {
            sQTETimeLeft = 0;
        }
    } else if (sQTETimeLeft == 0) {
        sQTETimeLeft = -1;
        this->frameTimer = RAND(QTE_ACTIVATE_TIME);
        set_play_mode(PLAY_MODE_NORMAL);
        gMarioState->health = 0;
    }
}

u8 chs_cond_quicktime(void) {
    return (
            !chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_A) &&
            !chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_B) &&
            !chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_Z) &&
            !chaos_check_if_patch_active(CHAOS_PATCH_BUTTON_BROKEN_C) &&
            !chaos_check_if_patch_active(CHAOS_PATCH_RED_LIGHT)
        );
}

void chs_act_quicktime(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_QUICKTIME, &this);
    this->frameTimer = RAND(QTE_ACTIVATE_TIME);
}

void chs_update_quicktime(void) {
    s32 group = (gMarioState->action & ACT_GROUP_MASK);
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_QUICKTIME, &this);

    if(this->frameTimer >= QTE_ACTIVATE_TIME && !(gMarioState->action & ACT_FLAG_INTANGIBLE) && group != ACT_GROUP_CUTSCENE) {
        generate_qte();
    }
}