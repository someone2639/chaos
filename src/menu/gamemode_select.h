#pragma once

#include <PR/ultratypes.h>
#include "types.h"
#include "game/chaos_menus.h"


#define GM_SELECT_DESC_Y        49
#define GM_START_GAME_X         (SCREEN_WIDTH - 43)
#define GM_START_GAME_Y         (GM_SELECT_DESC_Y + 45)
#define CHAL_SELECT_X           GM_START_GAME_X
#define CHAL_SELECT_Y           (GM_START_GAME_Y + 39)
#define DIFF_SELECT_X           GM_START_GAME_X
#define DIFF_SELECT_Y           (CHAL_SELECT_Y + 60)

#define GM_SELECT_DESC_Y_START    (GM_SELECT_DESC_Y - 100)
#define DIFF_SELECT_X_START       (SCREEN_WIDTH + 50)
#define CHAL_SELECT_X_START       (DIFF_SELECT_X_START + 50)
#define GM_START_GAME_X_START     (CHAL_SELECT_X_START + 50)

#define GM_SELECT_DIFF          0
#define GM_SELECT_CHAL          1
#define GM_SELECT_START         2

enum GamemodeSelectDescriptionID {
    GM_SELECT_DESC_DIFFICULTY,
    GM_SELECT_DESC_GAMEMODE,
    GM_SELECT_DESC_START,
    GM_SELECT_DESC_EASY,
    GM_SELECT_DESC_NORMAL,
    GM_SELECT_DESC_HARD,
    GM_SELECT_DESC_CLASSIC,
    GM_SELECT_DESC_CHALLENGE,
};

enum GamemodeSelectFlags {
    GAMEMODE_SELECT_FLAG_ACTIVE           = (1 << 0),
    GAMEMODE_SELECT_FLAG_HALT_INPUT       = (1 << 1),
    GAMEMODE_SELECT_FLAG_DRAW_MAIN_CURSOR = (1 << 2),
};

enum GamemodeSelectAnims {
    GM_SELECT_ANIM_STARTUP,
    GM_SELECT_ANIM_SELECTING,
    GM_SELECT_ANIM_CONFIRM,
    GM_SELECT_ANIM_RETURN,
};

enum GamemodeSelectState {
    GM_SELECT_STATE_DEFAULT,
    GM_SELECT_STATE_CHANGE_DIFF,
    GM_SELECT_STATE_CHANGE_CHALLENGE,
    GM_SELECT_STATE_CONFIRM,
    GM_SELECT_STATE_ENDING,
    GM_SELECT_STATE_RETURN,
};

struct GamemodeSelectMenu {
    struct ChaosMenu menu;
    u8 selectedDifficulty;
    u8 selectedChallenge;
    u8 prevSelection;
    Vec2f diffPos;
    Vec2f chalPos;
    Vec2f startGamePos;
    Vec2f descPos;
};

extern struct GamemodeSelectMenu sGamemodeSelectMenu;

void init_gamemode_select();
s32 update_gamemode_select();
void render_gamemode_select();
void gamemode_select_set_active();