#include <ultra64.h>
#include <PR/gbi.h>

#include "chaos_menus.h"
#include "sm64.h"
#include "game_init.h"
#include "segment2.h"

static u8 sDisplayButtonsDown = FALSE;

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

#define MENU_JOYSTICK_IGNORE_FRAMES         15      //Frames to wait before reading the same joystick input again
#define MENU_JOYSTICK_HOLD_SKIP_FRAMES      3       //Frames to skip reading the same joystick input while it is held

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
        stickDir |= MENU_JOYSTICK_DIR_UP;
    } else if(stickY < -60) {
        stickDir |= MENU_JOYSTICK_DIR_DOWN;
    } else if (stickX > 60) {
        stickDir |= MENU_JOYSTICK_DIR_RIGHT;
    } else if (stickX < -60) {
        stickDir |= MENU_JOYSTICK_DIR_LEFT;
    } else {
        menu->framesSinceLastStickInput = 0;
    }

    if(menu->framesSinceLastStickInput >= MENU_JOYSTICK_IGNORE_FRAMES 
        || stickDir != menu->lastStickDir)
    {
        menu->lastStickDir = stickDir;
        menu->framesSinceLastStickInput -= MENU_JOYSTICK_HOLD_SKIP_FRAMES;
    } else {
        stickDir = MENU_JOYSTICK_DIR_NONE;
    }
    
    if(menu->framesSinceLastStickInput < MENU_JOYSTICK_IGNORE_FRAMES) {
        menu->framesSinceLastStickInput++;
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

/*
    Sets up to draw a button prompt
*/
void menu_start_button_prompt() {
    //Alternate between showing the button sprite pressed and up every second
    if(!(gGlobalTimer % 30)) {
        sDisplayButtonsDown = !sDisplayButtonsDown;
    }
    gDPPipeSync(gDisplayListHead++);
	gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
	gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
	gDPSetAlphaCompare(gDisplayListHead++, G_AC_THRESHOLD);
	gDPSetBlendColor(gDisplayListHead++, 255, 255, 255, 255);
	gDPSetRenderMode(gDisplayListHead++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
    gDPPipeSync(gDisplayListHead++);
    gSPTexture(gDisplayListHead++, 65535, 65535, 0, 0, 1);
}

/*
    Resets everything after drawing a button prompt
*/
void menu_end_button_prompt() {
	gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
	gSPTexture(gDisplayListHead++, 65535, 65535, 0, G_TX_RENDERTILE, G_OFF);
	gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
	gDPSetAlphaCompare(gDisplayListHead++, G_AC_NONE);
	gDPSetRenderMode(gDisplayListHead++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
}

/*
    Draws a button prompt
*/
void menu_button_prompt(s32 x, s32 y, s32 button) {
    u8 *buttonTexture;

    switch(button) {
        case MENU_PROMPT_A_BUTTON:
            buttonTexture = (sDisplayButtonsDown) ? texture_icon_a_button_down : texture_icon_a_button;
            break;
        case MENU_PROMPT_B_BUTTON:
            buttonTexture = (sDisplayButtonsDown) ? texture_icon_b_button_down : texture_icon_b_button;
            break;
        case MENU_PROMPT_START_BUTTON:
            buttonTexture = (sDisplayButtonsDown) ? texture_icon_start_button_down : texture_icon_start_button;
            break;
        case MENU_PROMPT_L_TRIG:
            buttonTexture = (sDisplayButtonsDown) ? texture_icon_l_trig_down : texture_icon_l_trig;
            break;
        case MENU_PROMPT_R_TRIG:
            buttonTexture = (sDisplayButtonsDown) ? texture_icon_r_trig_down : texture_icon_r_trig;
            break;
        case MENU_PROMPT_Z_TRIG:
        default:
            buttonTexture = (sDisplayButtonsDown) ? texture_icon_z_trig_down : texture_icon_z_trig;
            break;
    }

    buttonTexture = segmented_to_virtual(buttonTexture);


    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, buttonTexture);
	gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 0, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0);
	gDPLoadBlock(gDisplayListHead++, 7, 0, 0, 255, 512);
	gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 4, 0, G_TX_WRAP | G_TX_NOMIRROR, 4, 0);
	gDPSetTileSize(gDisplayListHead++, 0, 0, 0, 60, 60);
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + 15) << 2, (y + 15) << 2,
                        G_TX_RENDERTILE, 0, 0, 4 << 10, 1 << 10);
    gDPPipeSync(gDisplayListHead++);
}