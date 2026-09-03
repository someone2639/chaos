#include <ultra64.h>
#include <PR/gbi.h>

#include "chaos_settings.h"
#include "fasttext.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "chaos_menus.h"
#include "chaos_pause_menu.h"
#include "patch_selection_ui.h"
#include "audio/external.h"
#include "sm64.h"
#include "main.h"
#include "save_file.h"

struct ChaosSettingsMenu gChaosSettingsMenu;

void squish_ui(Gfx **dlhead) {
    Gfx *gfx = *dlhead;
    if(gConfig.widescreen & WIDE_SCREEN_UI) {
        create_dl_translation_matrix(&gfx, MENU_MTX_NOPUSH, (SCREEN_WIDTH * (0.25f / 2)), 0.0f, 0.0f);
        create_dl_scale_matrix(&gfx, MENU_MTX_NOPUSH, 0.75f, 1.0f, 1.0f);
    }

    *dlhead = gfx;
}

s32 chaos_settings_get_widescreen(void) {
    s32 wideSetting = gConfig.widescreen;
    s32 selected;

    switch(wideSetting) {
        default:
        case WIDE_MODE_STANDARD:
            selected = 0;
            break;
        case WIDE_MODE_STRETCHED:
            selected = 1;
            break;
        case WIDE_MODE_FULL:
            selected = 2;
            break;
    }

    return selected;
}

void chaos_settings_set_widescreen(s32 selected) {
    s32 wideSetting = gConfig.widescreen;

    switch(selected) {
        case 0:
            wideSetting = WIDE_MODE_STANDARD;
            break;
        case 1:
            wideSetting = WIDE_MODE_STRETCHED;
            break;
        case 2:
            wideSetting = WIDE_MODE_FULL;
            break;
    }

    gConfig.widescreen = wideSetting;
    save_file_set_widescreen_mode(wideSetting);
}

struct ChaosSettingsOption sWideScreenOptions[] = {
    {
        .option = "Standard (4:3)",
        .description = "The default aspect ratio for Super Mario 64.",
    },
    {
        .option = "Stretched (16:9)",
        .description = "Stretched widescreen mode. Requires changing the settings of the emulator or TV display to support stretching the video."
    },
    {
        .option = "Full (16:9)",
        .description = "Same as the above, but also squishes some UI elements to provide a more consistent experience.\n"
        "@F03010--This mode will reduce the legibility of text if playing on console or at the game's native 240p resolution.@--------",
    },
};

s32 chaos_settings_get_music(void) {
    return gConfig.disableBGMusic;
}

void chaos_settings_set_music(s32 selected) {
    gConfig.disableBGMusic = selected;
    save_file_set_bg_music(gConfig.disableBGMusic);
}

struct ChaosSettingsOption sMusicOptions[] = {
    {
        .option = "Enable",
        .description = "Enables the in-game music.",
    },
    {
        .option = "Disable",
        .description = "Disables the in-game music.",
    },
};

s32 chaos_settings_get_sound_mode(void) {
    s32 soundMode = save_file_get_sound_mode();

    return soundMode;
}

void chaos_settings_set_sound_mode(s32 selected) {
    save_file_set_sound_mode(selected);
}

struct ChaosSettingsOption sSoundOptions[] = {
    {
        .option = "Stereo",
        .description = "The default \"STEREO\" sound mode from Super Mario 64. Sound output will be processed differently between the left and right sound channels, allowing for a directional perception of sound.",
    },
    {
        .option = "Mono",
        .description = "The \"MONO\" sound mode from Super Mario 64. The left and right sound channels will be mixed together, resulting in identical outputs for both channels.",
    },
    {
        .option = "Headset",
        .description = "The \"HEADSET\" sound mode from Super Mario 64. Behaves similarly to stereo, but sound effects are processed differently, with a less exaggerated directional emphasis.",
    },
};

s32 chaos_settings_get_harsh_visuals(void) {
    return gConfig.disableHarshVisuals;
}

void chaos_settings_set_harsh_visuals(s32 selected) {
    gConfig.disableHarshVisuals = selected;
    save_file_set_harsh_visuals_mode(gConfig.disableHarshVisuals);
}

struct ChaosSettingsOption sVisualsOptions[] = {
    {
        .option = "Enable All",
        .description = "Certain patches that could cause flashing, flickering lights, or are prone to motion sickness may appear as options in the patch selection menu.",
    },
    {
        .option = "Motion Only",
        .description = "Certain patches that are prone to motion sickness may appear as options in the patch selection menu. "
        "Patches containing flashing or flickering lights will not show up and have their currently active effects disabled.",
    },
    {
        .option = "Flashing Only",
        .description = "Certain patches containing flashing or flickering lights may appear as options in the patch selection menu. "
        "Patches that are prone to motion sickness will not show up and have their currently active effects disabled.",
    },
    {
        .option = "Disable All",
        .description = "Patches that could cause flashing, flickering lights, or are prone to motion sickness will no longer appear as options in the patch selection menu. "
        "Any currently applied patch that matches this criteria will have its effects disabled.",
    },
};

s32 chaos_settings_get_instant_input(void) {
    return (save_file_check_instant_input_active() == FALSE) ? 1 : 0;
}

void chaos_settings_set_instant_input(s32 selected) {
    save_file_set_instant_input_active((selected == 0) ? TRUE : FALSE);
}

struct ChaosSettingsOption sInstantInputOptions[] = {
    {
        .option = "Enable",
        .description = "Instant Input will be enabled. This may not take effect if a setup is determined to be incompatible (e.g. use of a real N64 console or a very high-accuracy emulator such as Ares is detected).",
    },
    {
        .option = "Disable",
        .description = "Instant Input will be disabled.",
    },
};

struct ChaosSettingsCategory gSettingsOptions[] = {
    {
        .display = "Widescreen Settings",
        .shortName = "Aspect Ratio",
        .description = "Enable/Disable the use of stretched widescreen.",
        .options = sWideScreenOptions,
        .numOptions = ARRAY_COUNT(sWideScreenOptions),
        .getConfig = chaos_settings_get_widescreen,
        .setConfig = chaos_settings_set_widescreen,
    },
    {
        .display = "Music Settings",
        .shortName = "Music",
        .description = "Enable/Disable in-game music.",
        .options = sMusicOptions,
        .numOptions = ARRAY_COUNT(sMusicOptions),
        .getConfig = chaos_settings_get_music,
        .setConfig = chaos_settings_set_music,
    },
    {
        .display = "Sound Settings",
        .shortName = "Sound Type",
        .description = "Change sound type.",
        .options = sSoundOptions,
        .numOptions = ARRAY_COUNT(sSoundOptions),
        .getConfig = chaos_settings_get_sound_mode,
        .setConfig = chaos_settings_set_sound_mode,
    },
    {
        .display = "Instant Input Settings",
        .shortName = "Instant Input",
        .description = "Instant Input can reduce up to 2 frames of visual latency on emulators using Framebuffer Emulation. @1FFF1F--This is recommended for most players@-------- that aren't used to the latency of console; "
        "however it can also lead to major rendering issues on some high-accuracy emulators @9F9F9F--(the effects of which will be immediately noticeable)@--------.",
        .options = sInstantInputOptions,
        .numOptions = ARRAY_COUNT(sInstantInputOptions),
        .getConfig = chaos_settings_get_instant_input,
        .setConfig = chaos_settings_set_instant_input,
    },
    {
        .display = "Harsh Visuals Settings",
        .shortName = "Harsh Visuals",
        .description = "Turn off some patch effects that may contain flashing, flickering lights, or are prone to motion sickness. Recommended for highly sensitive individuals, but will reduce overall patch variety.",
        .options = sVisualsOptions,
        .numOptions = ARRAY_COUNT(sVisualsOptions),
        .getConfig = chaos_settings_get_harsh_visuals,
        .setConfig = chaos_settings_set_harsh_visuals,
    },
};

/*
    Sets the default settings for the settings panel
*/
void init_settings_menu() {
    gChaosSettingsMenu.menu.flags |= (CHAOS_SETTINGS_ACTIVE | CHAOS_SETTINGS_HALT_INPUT);
    gChaosSettingsMenu.menu.animTimer = 0;
    gChaosSettingsMenu.menu.animFrames = MENU_ANIM_LOOP;
    gChaosSettingsMenu.menu.animId = CHAOS_SETTINGS_ANIM_APPEAR;
    gChaosSettingsMenu.menu.animPhase = 0;
    gChaosSettingsMenu.descY = SETTINGS_DESC_Y_START;
    gChaosSettingsMenu.catX = SETTINGS_CAT_X_START;
    gChaosSettingsMenu.optX = SETTINGS_OPT_X_START;
}

void render_settings_categories() {
    s32 index = gChaosSettingsMenu.index;

    create_dl_translation_matrix(&gDisplayListHead, MENU_MTX_PUSH, gChaosSettingsMenu.catX, SETTINGS_TOP_Y, 0);
    Gfx *catBg = menu_create_chaos_text_bg(SETTINGS_CAT_X, SETTINGS_TOP_Y, 196, 108, 217);
    gSPDisplayList(gDisplayListHead++, catBg);

    s32 yPos = 16;
    Gfx *catCursor = menu_create_cursor(-80, yPos - (15 * index) + 9, 0.5f, 0xFF, 0xFF, 0xFF, 0xFF);
    gSPDisplayList(gDisplayListHead++, catCursor);
    
    slowtext_setup_ortho_rendering(&gDisplayListHead, FT_FONT_SMALL_BOLD);
    slowtext_draw_ortho_text(&gDisplayListHead, 0, 34, "Settings", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_setup_ortho_rendering(&gDisplayListHead, FT_FONT_SMALL_THIN);
    
    for(int i = 0; i < ARRAY_COUNT(gSettingsOptions); i++) {
        slowtext_draw_ortho_text(&gDisplayListHead, -74, yPos, gSettingsOptions[i].display, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
        yPos -= 15;
    }
    slowtext_finished_rendering(&gDisplayListHead);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_settings_options() {
    s32 savedIndex = 0;
    s32 index = gChaosSettingsMenu.index;
    s32 subIndex = gChaosSettingsMenu.subIndex;
    struct ChaosSettingsCategory *selCat = &gSettingsOptions[index];
    s32 (*getConfig)(void) = selCat->getConfig;

    if(getConfig) {
        savedIndex = getConfig();
    }

    create_dl_translation_matrix(&gDisplayListHead, MENU_MTX_PUSH, gChaosSettingsMenu.optX, SETTINGS_TOP_Y, 0);
    Gfx *optBg = menu_create_chaos_text_bg(SETTINGS_OPT_X, SETTINGS_TOP_Y, 90, 108, 217);
    gSPDisplayList(gDisplayListHead++, optBg);

    s32 yPos = (s32)((selCat->numOptions - 1) * 7.5f) - 0;
    yPos = MIN(yPos, 15);

    if(gChaosSettingsMenu.menu.menuState == CHAOS_SETTINGS_STATE_SUB_MENU) {
        Gfx *optCursor = menu_create_cursor(-35, yPos - (15 * subIndex) + 9, 0.5f, 0xFF, 0xFF, 0xFF, 0xFF);
        gSPDisplayList(gDisplayListHead++, optCursor);
    }

    slowtext_setup_ortho_rendering(&gDisplayListHead, FT_FONT_SMALL_BOLD);
    slowtext_draw_ortho_text(&gDisplayListHead, 0, 34, selCat->shortName, FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_setup_ortho_rendering(&gDisplayListHead, FT_FONT_SMALL_THIN);
    for(int i = 0; i < selCat->numOptions; i++) {
        u8 col = (i == savedIndex) ? 0xFF : 0x7F;
        slowtext_draw_ortho_text(&gDisplayListHead, -30, yPos, selCat->options[i].option, FT_FLAG_ALIGN_LEFT, col, col, col, 0xFF);
        yPos -= 15;
    }
    slowtext_finished_rendering(&gDisplayListHead);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_settings_description() {
    s32 index = gChaosSettingsMenu.index;
    s32 subIndex = gChaosSettingsMenu.subIndex;
    struct ChaosSettingsCategory *selCat = &gSettingsOptions[index];

    create_dl_translation_matrix(&gDisplayListHead, MENU_MTX_PUSH, SETTINGS_DESC_X, gChaosSettingsMenu.descY, 0);
    Gfx *descBg = menu_create_chaos_text_bg(SETTINGS_DESC_X, SETTINGS_DESC_Y, 298, 70, 217);
    gSPDisplayList(gDisplayListHead++, descBg);
    
    slowtext_setup_ortho_rendering(&gDisplayListHead, FT_FONT_SMALL_THIN);
    char *desc = (gChaosSettingsMenu.menu.menuState == CHAOS_SETTINGS_STATE_SUB_MENU) ? selCat->options[subIndex].description : selCat->description;
    slowtext_draw_ortho_text_linebreaks(&gDisplayListHead, -142, 15, DESC_STRING_WIDTH, desc, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering(&gDisplayListHead);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Main rendering function for the settings menu
*/
void render_settings_menu() {
    shade_screen(&gDisplayListHead);
    squish_ui(&gDisplayListHead);
    render_settings_categories();
    render_settings_options();
    render_settings_description();
    
    if(!(gChaosSettingsMenu.menu.flags & CHAOS_SETTINGS_HALT_INPUT)) {
        render_settings_menu_button_prompts();
    }
}

/*
    Button prompts for the settings panel
*/
void render_settings_menu_button_prompts() {
    struct ButtonPromptList prompts = {0};
    menu_add_button_prompt(&prompts, MENU_PROMPT_A_BUTTON, "Select");
    menu_add_button_prompt(&prompts, MENU_PROMPT_B_BUTTON, "Back");
    menu_render_button_prompt_list(&gDisplayListHead, SCREEN_WIDTH - 32, PAUSE_BUTTON_PROMPTS_Y, &prompts);
}

/*
    Main input handling function for the settings panel
*/
void handle_settings_inputs() {
    if(gChaosSettingsMenu.menu.menuState == CHAOS_SETTINGS_STATE_MAIN_MENU) {
        if(gPlayer1Controller->buttonPressed & A_BUTTON) {
            gChaosSettingsMenu.menu.menuState = CHAOS_SETTINGS_STATE_SUB_MENU;
            gChaosSettingsMenu.subIndex = 0;
            play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
        } else if (gPlayer1Controller->buttonPressed & (B_BUTTON | L_TRIG | START_BUTTON)) {
            menu_play_anim(&gChaosSettingsMenu.menu, CHAOS_SETTINGS_ANIM_DISAPPEAR);
        } else if(menu_navigate_vertical(&gChaosSettingsMenu.index, 0, ARRAY_COUNT(gSettingsOptions), TRUE)) {
            play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
        }
    } else if (gChaosSettingsMenu.menu.menuState == CHAOS_SETTINGS_STATE_SUB_MENU) {
        struct ChaosSettingsCategory *selCat = &gSettingsOptions[gChaosSettingsMenu.index];
        if(gPlayer1Controller->buttonPressed & A_BUTTON) {
            void (*setFunc)(s32) = selCat->setConfig;
            if(setFunc) {
                setFunc(gChaosSettingsMenu.subIndex);
            }
        } else if (gPlayer1Controller->buttonPressed & B_BUTTON) {
            gChaosSettingsMenu.menu.menuState = CHAOS_SETTINGS_STATE_MAIN_MENU;
            play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
        } else if(menu_navigate_vertical(&gChaosSettingsMenu.subIndex, 0, selCat->numOptions, TRUE)) {
            play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
        }
    }
}

/*
    Starting animation for the settings panel
*/
#define SETTINGS_MENU_START_FRAMES   10
s32 chaos_settings_anim_appear() {
    s32 phase = gChaosSettingsMenu.menu.animPhase;

    if(!phase) {
        gChaosSettingsMenu.menu.animFrames = SETTINGS_MENU_START_FRAMES;
        gChaosSettingsMenu.menu.flags |= CHAOS_SETTINGS_HALT_INPUT;

        s32 animTimer = gChaosSettingsMenu.menu.animTimer;
        f32 prog = (f32)animTimer / (f32)gChaosSettingsMenu.menu.animFrames;

        if(animTimer == 0) {
            play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
        }

        gChaosSettingsMenu.descY = menu_anim_s32(prog, MENU_EASE_OUT, SETTINGS_DESC_Y_START, SETTINGS_DESC_Y);
        gChaosSettingsMenu.catX = menu_anim_s32(prog, MENU_EASE_OUT, SETTINGS_CAT_X_START, SETTINGS_CAT_X);
        gChaosSettingsMenu.optX = menu_anim_s32(prog, MENU_EASE_OUT, SETTINGS_OPT_X_START, SETTINGS_OPT_X);
    } else {
        gChaosSettingsMenu.menu.flags &= ~CHAOS_SETTINGS_HALT_INPUT;
        gChaosSettingsMenu.menu.animFrames = MENU_ANIM_LOOP;
    }
    return FALSE;
}

/*
    Ending animation for the settings panel
*/
#define SETTINGS_MENU_END_FRAMES   10
s32 chaos_settings_anim_disappear() {
    s32 phase = gChaosSettingsMenu.menu.animPhase;
    if(!phase) {
        gChaosSettingsMenu.menu.animFrames = SETTINGS_MENU_END_FRAMES;
        gChaosSettingsMenu.menu.flags |= CHAOS_SETTINGS_HALT_INPUT;

        s32 animTimer = gChaosSettingsMenu.menu.animTimer;
        f32 prog = (f32)animTimer / (f32)gChaosSettingsMenu.menu.animFrames;

        if(animTimer == 0) {
            play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
        }

        gChaosSettingsMenu.descY = menu_anim_s32(prog, MENU_EASE_IN, SETTINGS_DESC_Y, SETTINGS_DESC_Y_START);
        gChaosSettingsMenu.catX = menu_anim_s32(prog, MENU_EASE_IN, SETTINGS_CAT_X, SETTINGS_CAT_X_START);
        gChaosSettingsMenu.optX = menu_anim_s32(prog, MENU_EASE_IN, SETTINGS_OPT_X, SETTINGS_OPT_X_START);
    } else {
        gChaosSettingsMenu.menu.flags &= ~CHAOS_SETTINGS_ACTIVE;
        gChaosSettingsMenu.menu.animFrames = MENU_ANIM_LOOP;
    }
    return FALSE;
}

/*
    Settings panel animations
*/
s32 (*sSettingsPanelAnims[])(void) = {
    &chaos_settings_anim_appear,
    &chaos_settings_anim_disappear,
};

/*
    Main update function for the settings panel
*/
void update_settings_menu() {
    if(!(gChaosSettingsMenu.menu.flags & CHAOS_SETTINGS_HALT_INPUT)) {
        handle_settings_inputs();
    }

    menu_update_anims(&gChaosSettingsMenu.menu, sSettingsPanelAnims);
}