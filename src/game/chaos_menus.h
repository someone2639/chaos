#pragma once

#include <PR/ultratypes.h>
#include "types.h"
#include "src/engine/math_util.h"

#define MENU_ANIM_LOOP              -1

#define MENU_INPUT_IGNORE_FRAMES         15      // Frames to wait before reading the same input again
#define MENU_INPUT_HOLD_SKIP_FRAMES      3       // Frames to skip reading the same input while it is held

enum MenuEaseType {
    MENU_EASE_NONE,
    MENU_EASE_IN,
    MENU_EASE_OUT,
    MENU_EASE_BOTH,
};

enum MenuButtonPrompt {
    MENU_PROMPT_A_BUTTON,
    MENU_PROMPT_B_BUTTON,
    MENU_PROMPT_START_BUTTON,
    MENU_PROMPT_L_TRIG,
    MENU_PROMPT_R_TRIG,
    MENU_PROMPT_Z_TRIG,
    MENU_PROMPT_C_DOWN,
    MENU_PROMPT_C_LEFT,
    MENU_PROMPT_C_UP,
    MENU_PROMPT_C_RIGHT,
    MENU_PROMPT_COUNT,
};

enum MenuInputDir {
    MENU_DIR_NONE  =  0,

    MENU_DIR_U     = (1 << 0),
    MENU_DIR_D     = (1 << 1),
    MENU_DIR_R     = (1 << 2),
    MENU_DIR_L     = (1 << 3),

    MENU_DIR_UR    = (MENU_DIR_U | MENU_DIR_R),
    MENU_DIR_UL    = (MENU_DIR_U | MENU_DIR_L),
    MENU_DIR_DR    = (MENU_DIR_D | MENU_DIR_R),
    MENU_DIR_DL    = (MENU_DIR_D | MENU_DIR_L),
};

struct ButtonTexturePair {
    u8 *up;
    u8 *down;
};

struct ButtonPrompt {
    enum MenuButtonPrompt button;
    char *text;
    s32 offset;
    struct ButtonPrompt *next;
};

struct ButtonPromptList {
    struct ButtonPrompt *head;
    struct ButtonPrompt *tail;
};

//Generic menu handler
struct ChaosMenu {
    u32 flags;
    u8 menuState;
    s8 index;
    u16 animTimer;
    u16 animFrames;
    u8 animId;
    u8 animPhase;
};

void menu_play_anim(struct ChaosMenu *menu, s32 animId);
void menu_set_state(struct ChaosMenu *menu, u32 state);
void menu_update_input_dir();
u32 menu_get_input_dir();
s32 menu_navigate_horizontal(s32 *curIndex, s32 min, s32 max, s32 wrap);
s32 menu_navigate_vertical(s32 *curIndex, s32 min, s32 max, s32 wrap);
s32 menu_update_anims(struct ChaosMenu *menu, s32 (*animFunctions[])(void));
s32 menu_anim_s32(f32 prog, s32 easeType, s32 start, s32 end);
f32 menu_anim_f32(f32 prog, s32 easeType, f32 start, f32 end);

void menu_add_button_prompt(struct ButtonPromptList *list, enum MenuButtonPrompt button, char *text);
void menu_render_button_prompt_list(s32 x, s32 y, struct ButtonPromptList *list);
void menu_single_button_prompt(s32 x, s32 y, enum MenuButtonPrompt button, char *text, s32 alignLeft);

Gfx *menu_create_chaos_text_bg(s32 bgx, s32 bgy, s32 width, s32 height, u8 opacity);
Gfx *menu_create_cursor(s32 x, s32 y, f32 scale, u8 r, u8 g, u8 b, u8 a);

void menu_start_button();
void menu_end_button();
void menu_draw_button(s32 x, s32 y, s32 button, s32 pressed);