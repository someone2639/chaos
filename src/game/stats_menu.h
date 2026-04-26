#pragma once

#include <PR/ultratypes.h>
#include "sm64.h"
#include "chaos_menus.h"
#include "chaos_settings.h"
#include "main.h"

#define STATS_PADDING       12

#define STATS_TOTALS_X          ((SCREEN_CENTER_X / 2) + 7)
#define STATS_TOTALS_X_START    (STATS_TOTALS_X - 180 - WIDE_UI_EDGE_OFFSET)
#define STATS_TOTALS_Y          SCREEN_CENTER_Y

#define STATS_BEST_X            (((SCREEN_CENTER_X / 2) * 3) - 7)
#define STATS_BEST_X_START      (STATS_BEST_X + 180 + WIDE_UI_EDGE_OFFSET)
#define STATS_BEST_Y            (((SCREEN_HEIGHT / 3) * 2) - 7)

#define STATS_HARDCORE_X        (((SCREEN_CENTER_X / 2) * 3) - 7)
#define STATS_HARDCORE_X_START  (STATS_HARDCORE_X + 180 + WIDE_UI_EDGE_OFFSET)
#define STATS_HARDCORE_Y        (((SCREEN_HEIGHT / 6)) + 17)

enum StatsMenuFlags {
    CHAOS_STATS_ACTIVE      = (1 << 0),
    CHAOS_STATS_HALT_INPUT  = (1 << 1),
    CHAOS_STATS_DRAW_CONF   = (1 << 2),
};

enum StatsMenuState {
    STATS_STATE_DEFAULT,
    STATS_STATE_CONF_DELETE,
};

enum StatsMenuAnims {
    STATS_ANIM_OPEN,
    STATS_ANIM_CLOSE,
    STATS_ANIM_CONF_APPEAR,
    STATS_ANIM_CONF_DISAPPEAR,
};

struct StatsMenu {
    struct ChaosMenu menu;
    u8 delTimer;
    s16 totalsX;
    s16 bestX;
    s16 hardcoreX;
    f32 confScale;
};

extern struct StatsMenu gChaosStatsMenu;

void init_stats_menu();
void update_stats_menu();
void render_stats_menu();
