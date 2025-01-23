#include <PR/ultratypes.h>
#include "types.h"
#include "src/engine/math_util.h"
#include "chaos_menus.h"
#include "chaos/chaos.h"

#define MINI_CARD_X                 74
#define MINI_CARD_X_START           -104
#define ACTIVE_PATCH_DESC_X         (SCREEN_WIDTH - 74)
#define ACTIVE_PATCH_DESC_X_START   (SCREEN_WIDTH + 104)
#define ACTIVE_PATCH_DESC_Y         (SCREEN_HEIGHT / 3)

enum ChaosSettingsFlags {
    CHAOS_SETTINGS_ACTIVE           = (1 << 0),
};

enum ActivePatchesMenuFlags {
    ACTIVE_PATCHES_MENU_ACTIVE              = (1 << 0),
    ACTIVE_PATCHES_MENU_STOP_GAME_RENDER    = (1 << 1),
    ACTIVE_PATCHES_MENU_HALT_INPUT          = (1 << 2),
    ACTIVE_PATCHES_MENU_DRAW_EXT_DESC       = (1 << 3),
};

enum ActivePatchesMenuAnims {
    ACTIVE_PATCHES_MENU_ANIM_STARTUP,
    ACTIVE_PATCHES_MENU_ANIM_ENDING,
    ACTIVE_PATCHES_MENU_ANIM_EXT_DESC_APPEAR,
    ACTIVE_PATCHES_MENU_ANIM_EXT_DESC_DISAPPEAR,
};

enum ActivePatchesMenuState {
    ACTIVE_PATCHES_MENU_STATE_DEFAULT,
    ACTIVE_PATCHES_MENU_STATE_SHOW_EXT_DESC,
};

struct ChaosPauseMenu {
    struct ChaosMenu settingsMenu;
    struct ChaosMenu activePatchesMenu;
    u16 chaosListStart;
    f32 descX;
    f32 cardX;
    f32 extDescScale;
};

extern struct ChaosPauseMenu *gChaosPauseMenu;

void render_settings_panel();
void handle_settings_inputs();
void render_active_patches();
void update_active_patches_menu();
void init_active_patches_menu();