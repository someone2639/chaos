#pragma once

#include <PR/ultratypes.h>
#include "types.h"
#include "src/engine/math_util.h"
#include "chaos_menus.h"
#include "chaos/chaos.h"
#include "chaos_pause_menu.h"

#define WIDE_UI_EDGE_OFFSET         ((gConfig.widescreen & WIDE_SCREEN_UI) ? 50 : 0)

#define WIDE_MODE_STANDARD          (WIDE_SCREEN_DISABLED)
#define WIDE_MODE_STRETCHED         (WIDE_SCREEN_ENABLED)
#define WIDE_MODE_FULL              (WIDE_SCREEN_ENABLED | WIDE_SCREEN_UI)

#define SETTINGS_DESC_X         SCREEN_CENTER_X

#define SETTINGS_CAT_X          (SETTINGS_DESC_X - 51)
#define SETTINGS_CAT_X_START    (-110 - WIDE_UI_EDGE_OFFSET)

#define SETTINGS_OPT_X          (SETTINGS_DESC_X + 104)
#define SETTINGS_OPT_X_START    (SCREEN_WIDTH + 60 + WIDE_UI_EDGE_OFFSET)

#define SETTINGS_TOP_Y          ((SCREEN_HEIGHT / 3) * 2)

#define SETTINGS_DESC_Y         (59)
#define SETTINGS_DESC_Y_START   (-60)

enum ChaosSettingsFlags {
    CHAOS_SETTINGS_ACTIVE           = (1 << 0),
    CHAOS_SETTINGS_HALT_INPUT       = (1 << 1),
};

enum ChaosSettingsAnims {
    CHAOS_SETTINGS_ANIM_APPEAR,
    CHAOS_SETTINGS_ANIM_DISAPPEAR,
};

enum ChaosSettingsState {
    CHAOS_SETTINGS_STATE_MAIN_MENU,
    CHAOS_SETTINGS_STATE_SUB_MENU,
};

struct ChaosSettingsMenu {
    struct ChaosMenu menu;
    s32 index;
    s32 subIndex;
    s32 descY;
    s32 catX;
    s32 optX;
};

struct ChaosSettingsOption {
    char *option;
    char *description;
};

struct ChaosSettingsCategory {
    char *display;
    char *shortName;
    char *description;
    struct ChaosSettingsOption *options;
    s16 numOptions;
    s32 (*getConfig)(void); // Function that returns which menu index corresponds to the currently saved config option
    void (*setConfig)(s32); // Function that saves the config option that corresponds to the currently selected menu index
};

extern struct ChaosSettingsCategory gSettingsOptions[];
extern struct ChaosSettingsMenu gChaosSettingsMenu;

void render_settings_menu(void);
void update_settings_menu(void);
void init_settings_menu(void);
void render_settings_menu_button_prompts(void);

void squish_ui(Gfx **dlhead);