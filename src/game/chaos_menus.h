#pragma once

#include <PR/ultratypes.h>
#include "types.h"
#include "src/engine/math_util.h"

#define MENU_ANIM_LOOP              -1

enum MenuButtonPrompt {
    MENU_PROMPT_A_BUTTON,
    MENU_PROMPT_B_BUTTON,
    MENU_PROMPT_START_BUTTON,
    MENU_PROMPT_L_TRIG,
    MENU_PROMPT_R_TRIG,
    MENU_PROMPT_Z_TRIG,
};

enum MenuJoystickDir {
    MENU_JOYSTICK_DIR_NONE  =  0,
    MENU_JOYSTICK_DIR_UP    = (1 << 0),
    MENU_JOYSTICK_DIR_DOWN  = (1 << 1),
    MENU_JOYSTICK_DIR_RIGHT = (1 << 2),
    MENU_JOYSTICK_DIR_LEFT  = (1 << 3),
};

//Generic menu handler
struct ChaosMenu {
    u32 flags;
    u8 menuState;
    s8 selectedMenuIndex;
    s16 framesSinceLastStickInput;
    u8 lastStickDir;
    u16 animTimer;
    u16 animFrames;
    u8 animId;
    u8 animPhase;
};

f32 menu_translate_percentage(f32 start, f32 end, f32 percent);
void menu_play_anim(struct ChaosMenu *menu, s32 animId);
void menu_set_state(struct ChaosMenu *menu, u32 state);
u32 menu_update_joystick_dir(struct ChaosMenu *menu);
s32 menu_update_anims(struct ChaosMenu *menu, s32 (*animFunctions[])(void));
void menu_start_button_prompt();
void menu_end_button_prompt();
void menu_button_prompt(s32 x, s32 y, s32 button);