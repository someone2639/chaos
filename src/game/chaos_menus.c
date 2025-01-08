#include <ultra64.h>
#include <PR/gbi.h>

#include "chaos_menus.h"
#include "sm64.h"
#include "game_init.h"

/*
    Returns the point a percentage of the way from start to end, used for menu anims
*/
f32 menu_translate_percentage(f32 start, f32 end, f32 percent) {
    f32 point = start + (end - start) * percent;
    return point;
}

/*
    Sets the menu anim id and resets timer and phase
*/
void menu_play_anim(struct ChaosMenu *menu, s32 animId){
    menu->animTimer = 0;
    menu->animPhase = 0;
    menu->animId = animId;
}

/*
    Updates the menu state
*/
void menu_set_state(struct ChaosMenu *menu, u32 state) {
    menu->menuState = state;
}

/*
    Handles joystick navigation for menus. Updates the last held stick direction and returns either 
    the stick direction or MENU_JOYSTICK_DIR_NONE if the direction is the same as the 
    last direction, unless a certain number of frames has passed.
*/
u32 menu_update_joystick_dir(struct ChaosMenu *menu) {
    f32 stickX = gPlayer1Controller->rawStickX;
    f32 stickY = gPlayer1Controller->rawStickY;
    u32 stickDir = MENU_JOYSTICK_DIR_NONE;

    if(stickY > 60) {
        stickDir = MENU_JOYSTICK_DIR_UP;
    } else if(stickY < -60) {
        stickDir = MENU_JOYSTICK_DIR_DOWN;
    } else if (stickX > 60) {
        stickDir = MENU_JOYSTICK_DIR_RIGHT;
    } else if (stickX < -60) {
        stickDir = MENU_JOYSTICK_DIR_LEFT;
    }

    if(menu->framesSinceLastStickInput >= MENU_JOYSTICK_HOLD_FRAMES 
        || stickDir != menu->lastStickDir)
    {
        menu->lastStickDir = stickDir;
    } else {
        stickDir = MENU_JOYSTICK_DIR_NONE;
    }
    
    if(stickDir != MENU_JOYSTICK_DIR_NONE){
        menu->framesSinceLastStickInput = 0;
    } else {
        if(menu->framesSinceLastStickInput < MENU_JOYSTICK_HOLD_FRAMES) {
            menu->framesSinceLastStickInput++;
        }
    }

    return stickDir;
}

/*
    Updates the animations for a menu. Returns true if the animation has finished.
*/
s32 menu_update_anims(struct ChaosMenu *menu, s32 (*animFunctions[])(void)) {
    s32 animId = menu->animId;
    s32 animFrames = menu->animFrames;

    s32 (*animFunc)(void) = animFunctions[animId];
    s32 isFinished = animFunc();

    if(isFinished) {
        return TRUE;
    } else {
        if(++menu->animTimer > animFrames) {
            menu->animTimer = 0;
            menu->animPhase++;
        }
        return FALSE;
    }
}
