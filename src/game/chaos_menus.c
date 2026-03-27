#include <ultra64.h>
#include <PR/gbi.h>

#include "chaos_menus.h"
#include "sm64.h"
#include "game_init.h"
#include "segment2.h"
#include "fasttext.h"
#include "debug.h"

struct ButtonTexturePair sButtonTextureTable[MENU_PROMPT_COUNT] = {
    [MENU_PROMPT_A_BUTTON]      = {.up = texture_icon_a_button,     .down = texture_icon_a_button_down},
    [MENU_PROMPT_B_BUTTON]      = {.up = texture_icon_b_button,     .down = texture_icon_b_button_down},
    [MENU_PROMPT_START_BUTTON]  = {.up = texture_icon_start_button, .down = texture_icon_start_button_down},
    [MENU_PROMPT_L_TRIG]        = {.up = texture_icon_l_trig,       .down = texture_icon_l_trig_down},
    [MENU_PROMPT_R_TRIG]        = {.up = texture_icon_r_trig,       .down = texture_icon_r_trig_down},
    [MENU_PROMPT_Z_TRIG]        = {.up = texture_icon_z_trig,       .down = texture_icon_z_trig_down},
    [MENU_PROMPT_C_DOWN]        = {.up = texture_icon_c_d_button,   .down = texture_icon_c_d_button_down},
    [MENU_PROMPT_C_LEFT]        = {.up = texture_icon_c_l_button,   .down = texture_icon_c_l_button_down},
    [MENU_PROMPT_C_UP]          = {.up = texture_icon_c_u_button,   .down = texture_icon_c_u_button_down},
    [MENU_PROMPT_C_RIGHT]       = {.up = texture_icon_c_r_button,   .down = texture_icon_c_r_button_down},
};

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
    Sets up to draw a button texture
*/
void menu_start_button() {
    gDPPipeSync(gDisplayListHead++);
	gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
	gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
	gDPSetAlphaCompare(gDisplayListHead++, G_AC_THRESHOLD);
	gDPSetBlendColor(gDisplayListHead++, 255, 255, 255, 255);
    gDPPipeSync(gDisplayListHead++);
    gSPTexture(gDisplayListHead++, 65535, 65535, 0, 0, 1);
}

/*
    Resets everything after drawing a button texture
*/
void menu_end_button() {
    gDPPipeSync(gDisplayListHead++);
	gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
	gSPTexture(gDisplayListHead++, 65535, 65535, 0, G_TX_RENDERTILE, G_OFF);
	gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
	gDPSetAlphaCompare(gDisplayListHead++, G_AC_NONE);
}

/*
    Draws a button texture
*/
void menu_draw_button(s32 x, s32 y, s32 button, s32 pressed) {
    u8 *buttonTexture = (pressed) ? sButtonTextureTable[button].down : sButtonTextureTable[button].up;
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

/*
    Adds a button prompt to the button prompt list
*/
void menu_add_button_prompt(struct ButtonPromptList *list, enum MenuButtonPrompt button, char *text) {
    struct ButtonPrompt *prompt = alloc_display_list(sizeof(struct ButtonPrompt));
    prompt->button = button;
    prompt->text = text;
    prompt->next = NULL;

    // Get width of text
    int xPos = 0;
    int i = 0;

    if (!text) {
        assert(FALSE, "Attempted to pass NULL string to button prompt func!");
    }

    const struct FastTextProps *fontProps = &gFasttextFonts[FT_FONT_SMALL_THIN];
    const unsigned char *kerningTable = fontProps->kerningTable;

    while (text[i] != '\0' && text[i] != '\n') {
        int curChar = text[i];
        int charIndex = curChar - ' ';
        int tabCount;

        switch (curChar) {
            case '\t': // tab
                tabCount = (xPos + FT_TAB_WIDTH) / FT_TAB_WIDTH;
                xPos = tabCount * FT_TAB_WIDTH;
                break;
            case '@': // string color
                i += 8;
                break;
            default:
                xPos += kerningTable[charIndex];
                break;
        }

        i++;
    }

    xPos -= fontProps->subFromLength;
    if (xPos < 0) {
        xPos = 0;
    }

    prompt->offset = xPos + 22;

    if(list->tail) {
        list->tail->next = prompt;
    }
    if(!list->head) {
        list->head = prompt;
    }
    list->tail = prompt;
}

/*
    Renders the contents of a button prompt list. First draws all the buttons, then all the text.
    Always draws right aligned button prompts, going horizontally to the left.
*/
void menu_render_button_prompt_list(s32 x, s32 y, struct ButtonPromptList *list) {
    s32 pressed = ((gGlobalTimer % 60) > 30);
    // Draw buttons
    menu_start_button();
    s32 offset = 0;
    for(struct ButtonPrompt *prompt = list->head; prompt != NULL; prompt = prompt->next) {
        menu_draw_button(x + offset, y, prompt->button, pressed);
        offset -= prompt->offset;
    }
    menu_end_button();

    // Draw text
    fasttext_setup_textrect_rendering(FT_FONT_SMALL_THIN);
    offset = -2;
    for(struct ButtonPrompt *prompt = list->head; prompt != NULL; prompt = prompt->next) {
        fasttext_draw_texrect(x + offset, y, prompt->text, FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
        offset -= prompt->offset;
    }
    fasttext_finished_rendering();
}

/*
    Draws a single button prompt
*/
void menu_single_button_prompt(s32 x, s32 y, enum MenuButtonPrompt button, char *text, s32 alignLeft) {
    s32 pressed = ((gGlobalTimer % 60) > 30);

    menu_start_button();
    menu_draw_button(x, y, button, pressed);
    menu_end_button();

    s32 offset, align;
    if(alignLeft) {
        offset = 18;
        align = FT_FLAG_ALIGN_LEFT;
    } else {
        offset = -2;
        align = FT_FLAG_ALIGN_RIGHT;
    }

    fasttext_setup_textrect_rendering(FT_FONT_SMALL_THIN);
    fasttext_draw_texrect(x + offset, y, text, align, 0xFF, 0xFF, 0xFF, 0xFF);
    fasttext_finished_rendering();
}
