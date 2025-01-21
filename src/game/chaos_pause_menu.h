#include <PR/ultratypes.h>
#include "types.h"
#include "src/engine/math_util.h"
#include "chaos_menus.h"
#include "chaos/chaos.h"

enum ChaosSettingsFlags {
    CHAOS_SETTINGS_ACTIVE           = (1 << 0),
};

struct ChaosPauseMenu {
    struct ChaosMenu settingsMenu;
    //struct ChaosMenu activePatchesMenu;
    u8 selectedAspectRatio;
    u8 selectedMusic;
};

extern struct ChaosPauseMenu *gChaosPauseMenu;

void render_settings_panel();
void handle_settings_inputs();