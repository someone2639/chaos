#pragma once

#include <PR/ultratypes.h>
#include "types.h"
#include "game/chaos_menus.h"

#define GM_SELECT_DESC_Y        49
#define CHAL_SELECT_X           (SCREEN_WIDTH - 43)
#define CHAL_SELECT_Y           (GM_SELECT_DESC_Y + 67)
#define DIFF_SELECT_X           CHAL_SELECT_X
#define DIFF_SELECT_Y           (CHAL_SELECT_Y + 70)
#define GM_SELECT_DIFF          0
#define GM_SELECT_CHAL          1

enum GamemodeSelectDescriptionID {
    GM_SELECT_DESC_DIFFICULTY,
    GM_SELECT_DESC_GAMEMODE,
    GM_SELECT_DESC_EASY,
    GM_SELECT_DESC_NORMAL,
    GM_SELECT_DESC_HARD,
    GM_SELECT_DESC_CLASSIC,
    GM_SELECT_DESC_CHALLENGE,
};

enum GamemodeSelectFlags {
    GAMEMODE_SELECT_FLAG_HALT_INPUT,
};

enum GamemodeSelectState {
    GM_SELECT_STATE_DEFAULT,
    GM_SELECT_STATE_CHANGE_DIFF,
    GM_SELECT_STATE_CHANGE_CHALLENGE,
    GM_SELECT_STATE_CONFIRM,
};

struct GamemodeSelectMenu {
    struct ChaosMenu menu;
    u8 selectedDifficulty;
    u8 selectedChallenge;
    u8 prevSelection;
};

void init_gamemode_select();
void update_gamemode_select();
void render_gamemode_select();