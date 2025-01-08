#pragma once

#include <PR/ultratypes.h>
#include "types.h"
#include "src/engine/math_util.h"

#define MENU_JOYSTICK_HOLD_FRAMES   10
#define MENU_ANIM_LOOP              -1

enum MenuJoystickDir {
    MENU_JOYSTICK_DIR_NONE,
    MENU_JOYSTICK_DIR_UP,
    MENU_JOYSTICK_DIR_DOWN,
    MENU_JOYSTICK_DIR_RIGHT,
    MENU_JOYSTICK_DIR_LEFT,
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