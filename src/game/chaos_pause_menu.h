#include <PR/ultratypes.h>
#include "types.h"
#include "src/engine/math_util.h"
#include "chaos_menus.h"
#include "chaos/chaos.h"

enum ChaosSettingsFlags {
    CHAOS_SETTINGS_ACTIVE           = (1 << 0),
};

enum ActivePatchesMenuFlags {
    ACTIVE_PATCHES_MENU_ACTIVE      = (1 << 0),
};

struct ChaosPauseMenu {
    struct ChaosMenu settingsMenu;
    struct ChaosMenu activePatchesMenu;
    u16 chaosListStart;
};

extern struct ChaosPauseMenu *gChaosPauseMenu;

void render_settings_panel();
void handle_settings_inputs();
void render_active_patches();
void handle_active_patches_inputs();