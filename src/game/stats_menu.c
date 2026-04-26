#include "stats_menu.h"
#include "chaos_menus.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "fasttext.h"
#include "patch_selection_ui.h"
#include "save_file.h"
#include "audio/external.h"

extern struct SaveBuffer gSaveBuffer;

struct StatsMenu gChaosStatsMenu = {0};

void init_stats_menu() {
    menu_set_state(&gChaosStatsMenu.menu, STATS_STATE_DEFAULT);
    gChaosStatsMenu.menu.flags = (CHAOS_STATS_ACTIVE | CHAOS_STATS_HALT_INPUT);
    gChaosStatsMenu.delTimer = 0;
    gChaosStatsMenu.totalsX = STATS_TOTALS_X_START;
    gChaosStatsMenu.bestX = STATS_BEST_X_START;
    gChaosStatsMenu.hardcoreX = STATS_HARDCORE_X_START;
    gChaosStatsMenu.confScale = 0.0f;

    gChaosStatsMenu.menu.animTimer = 0;
    gChaosStatsMenu.menu.animFrames = MENU_ANIM_LOOP;
    gChaosStatsMenu.menu.animId = STATS_ANIM_OPEN;
    gChaosStatsMenu.menu.animPhase = 0;
}

/*
    Anims
*/

#define STATS_ANIM_OPEN_FRAMES  10
s32 stats_anim_open() {
    s32 phase = gChaosStatsMenu.menu.animPhase;
    struct ChaosMenu *menu = &gChaosStatsMenu.menu;
    if(!phase) {
        menu->animFrames = STATS_ANIM_OPEN_FRAMES;
        menu->flags |= CHAOS_STATS_HALT_INPUT;

        f32 prog = menu_get_anim_prog(menu);

        gChaosStatsMenu.totalsX = menu_anim_s32((prog * 1.8f), MENU_EASE_OUT, STATS_TOTALS_X_START, STATS_TOTALS_X);
        gChaosStatsMenu.bestX = menu_anim_s32((prog * 1.4f), MENU_EASE_OUT, STATS_BEST_X_START, STATS_BEST_X);
        gChaosStatsMenu.hardcoreX = menu_anim_s32(prog, MENU_EASE_OUT, STATS_HARDCORE_X_START, STATS_HARDCORE_X);

        if(menu->animTimer == 0) {
            play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
        }
    } else {
        menu->flags &= ~CHAOS_STATS_HALT_INPUT;
        menu->animFrames = MENU_ANIM_LOOP;
    }

    return FALSE;
}

#define STATS_ANIM_CLOSE_FRAMES  10
s32 stats_anim_close() {
    s32 phase = gChaosStatsMenu.menu.animPhase;
    struct ChaosMenu *menu = &gChaosStatsMenu.menu;
    if(!phase) {
        menu->animFrames = STATS_ANIM_CLOSE_FRAMES;
        menu->flags |= CHAOS_STATS_HALT_INPUT;

        f32 prog = menu_get_anim_prog(menu);

        gChaosStatsMenu.totalsX = menu_anim_s32(prog, MENU_EASE_IN, STATS_TOTALS_X, STATS_TOTALS_X_START);
        gChaosStatsMenu.bestX = menu_anim_s32((prog * 1.4f), MENU_EASE_IN, STATS_BEST_X, STATS_BEST_X_START);
        gChaosStatsMenu.hardcoreX = menu_anim_s32((prog * 1.8f), MENU_EASE_IN, STATS_HARDCORE_X, STATS_HARDCORE_X_START);
    } else {
        menu->flags &= ~CHAOS_STATS_ACTIVE;
        menu->animFrames = MENU_ANIM_LOOP;
    }

    return FALSE;
}

#define STATS_ANIM_CONF_APPEAR_FRAMES   7
s32 stats_anim_conf_appear() {
    s32 phase = gChaosStatsMenu.menu.animPhase;
    struct ChaosMenu *menu = &gChaosStatsMenu.menu;
    if(!phase) {
        menu->animFrames = STATS_ANIM_CONF_APPEAR_FRAMES;
        menu->flags |= (CHAOS_STATS_HALT_INPUT | CHAOS_STATS_DRAW_CONF);

        f32 prog = menu_get_anim_prog(menu);

        gChaosStatsMenu.confScale = menu_anim_f32(prog, MENU_EASE_OUT, 0.0f, 1.0f);
    } else {
        menu->flags &= ~CHAOS_STATS_HALT_INPUT;
        menu->animFrames = MENU_ANIM_LOOP;
    }
    return FALSE;
}

#define STATS_ANIM_CONF_DISAPPEAR_FRAMES   7
s32 stats_anim_conf_disappear() {
    s32 phase = gChaosStatsMenu.menu.animPhase;
    struct ChaosMenu *menu = &gChaosStatsMenu.menu;
    if(!phase) {
        menu->animFrames = STATS_ANIM_CONF_DISAPPEAR_FRAMES;
        menu->flags |= CHAOS_STATS_HALT_INPUT;

        f32 prog = menu_get_anim_prog(menu);

        gChaosStatsMenu.confScale = menu_anim_f32(prog, MENU_EASE_IN, 1.0f, 0.0f);
    } else {
        menu->flags &= ~(CHAOS_STATS_HALT_INPUT | CHAOS_STATS_DRAW_CONF);
        menu->animFrames = MENU_ANIM_LOOP;
    }
    return FALSE;
}

s32 (*sStatsMenuAnims[])(void) = {
    [STATS_ANIM_OPEN]           = &stats_anim_open,
    [STATS_ANIM_CLOSE]          = &stats_anim_close,
    [STATS_ANIM_CONF_APPEAR]    = &stats_anim_conf_appear,
    [STATS_ANIM_CONF_DISAPPEAR] = &stats_anim_conf_disappear,
};

/*
    Control
*/

void stats_state_default() {
    if(gPlayer1Controller->buttonPressed & B_BUTTON) {
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
        menu_play_anim(&gChaosStatsMenu.menu, STATS_ANIM_CLOSE);
    }

    if(gPlayer1Controller->buttonDown & START_BUTTON) {
        if(++gChaosStatsMenu.delTimer > 60) {
            gChaosStatsMenu.menu.index = 1;
            gChaosStatsMenu.delTimer = 0;
            menu_set_state(&gChaosStatsMenu.menu, STATS_STATE_CONF_DELETE);
            play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
            menu_play_anim(&gChaosStatsMenu.menu, STATS_ANIM_CONF_APPEAR);
        }
    } else {
        gChaosStatsMenu.delTimer = 0;
    }
}

void stats_state_conf_delete() {
    s32 index = gChaosStatsMenu.menu.index;
    if((gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) && (index == 0)) {
        save_file_delete_stats();
        menu_set_state(&gChaosStatsMenu.menu, STATS_STATE_DEFAULT);
        play_sound(SOUND_MARIO_WAAAOOOW, gGlobalSoundSource);
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
        menu_play_anim(&gChaosStatsMenu.menu, STATS_ANIM_CONF_DISAPPEAR);
    } else if((gPlayer1Controller->buttonPressed & B_BUTTON) || ((gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) && (index == 1))) {
        menu_set_state(&gChaosStatsMenu.menu, STATS_STATE_DEFAULT);
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
        menu_play_anim(&gChaosStatsMenu.menu, STATS_ANIM_CONF_DISAPPEAR);
    } else if(menu_navigate_horizontal(&index, 0, 2, TRUE)) {
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    }

    gChaosStatsMenu.menu.index = index;
}

void update_stats_menu() {
    if(!(gChaosStatsMenu.menu.flags & CHAOS_STATS_HALT_INPUT)) {
        switch(gChaosStatsMenu.menu.menuState) {
            case STATS_STATE_DEFAULT:
                stats_state_default();
                break;
            case STATS_STATE_CONF_DELETE:
                stats_state_conf_delete();
                break;
        }
    }

    menu_update_anims(&gChaosStatsMenu.menu, sStatsMenuAnims);
}

/*
    Draw
*/

ALWAYS_INLINE void stats_menu_write_playtime_stat(s32 leftAlign, s32 rightAlign, s32 yPos, const char *label, u32 time) {
    char playTimeText[32];
    u32 playTime = time;
    u32 days, hours, mins, secs;

    days = playTime / (30 * 60 * 60 * 24);
    playTime -= days * (30 * 60 * 60 * 24);

    hours = playTime / (30 * 60 * 60);
    playTime -= hours * (30 * 60 * 60);

    mins = playTime / (60 * 30);
    playTime -= mins * (60 * 30);

    secs = playTime / 30;

    sprintf(playTimeText, "%dd %dh %dm %ds", days, hours, mins, secs);

    slowtext_draw_ortho_text(leftAlign, yPos, label, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(rightAlign, yPos, playTimeText, FT_FLAG_ALIGN_RIGHT, 0x9F, 0x9F, 0x9F, 0xFF);
}

ALWAYS_INLINE void stats_menu_write_basic_stat(s32 leftAlign, s32 rightAlign, s32 yPos, const char *label, u32 stat, u8 r, u8 g, u8 b) {
    char statText[32];
    sprintf(statText, "%d", stat);

    slowtext_draw_ortho_text(leftAlign, yPos, label, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(rightAlign, yPos, statText, FT_FLAG_ALIGN_RIGHT, r, g, b, 0xFF);
}

ALWAYS_INLINE void stats_menu_write_difficulty(s32 leftAlign, s32 rightAlign, s32 yPos, u32 difficulty) {
    slowtext_draw_ortho_text(leftAlign, yPos, "Difficulty:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    switch(difficulty) {
        case CHAOS_DIFFICULTY_EASY:
            slowtext_draw_ortho_text(rightAlign, yPos, "Easy", FT_FLAG_ALIGN_RIGHT, 0x05, 0xDf, 0x15, 0xFF);
            break;
        case CHAOS_DIFFICULTY_NORMAL:
        default:
            slowtext_draw_ortho_text(rightAlign, yPos, "Normal", FT_FLAG_ALIGN_RIGHT, 0xAF, 0xAF, 0xAF, 0xFF);
            break;
        case CHAOS_DIFFICULTY_HARD:
            slowtext_draw_ortho_text(rightAlign, yPos, "Hard", FT_FLAG_ALIGN_RIGHT, 0xFF, 0x15, 0x25, 0xFF);
            break;
        case CHAOS_DIFFICULTY_IMPOSSIBLE:
            slowtext_draw_ortho_text(rightAlign, yPos, "Impossible", FT_FLAG_ALIGN_RIGHT, 0x5F, 0x5F, 0x5F, 0xFF);
            break;
    }
}

ALWAYS_INLINE void stats_menu_write_favorite_patch(s32 leftAlign, s32 yPos, s32 width, const char *label, s32 patchId, u8 r, u8 g, u8 b) {
    char favText[64];
    const struct ChaosPatch *favPatch = &gChaosPatches[patchId];
    s32 count = gSaveBuffer.menuData.scoreData.picked[patchId];

    slowtext_draw_ortho_text(leftAlign, yPos, label, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    if(patchId == CHAOS_PATCH_NONE_POSITIVE || patchId == CHAOS_PATCH_NONE_NEGATIVE) {
        slowtext_draw_ortho_text_linebreaks(leftAlign + 15, yPos -= STATS_PADDING, width, "---", FT_FLAG_ALIGN_LEFT, 0x7F, 0x7F, 0x7F, 0xFF);
    } else {
        sprintf(favText, "%s (%d)", favPatch->name, count);
        slowtext_draw_ortho_text_linebreaks(leftAlign + 15, yPos -= STATS_PADDING, width, favText, FT_FLAG_ALIGN_LEFT, r, g, b, 0xFF);
    }
}

void render_stats_menu_totals() {
    s32 x = gChaosStatsMenu.totalsX;
    s32 y = STATS_TOTALS_Y;
    s32 w = (SCREEN_WIDTH / 2) - 15;
    s32 h = (SCREEN_HEIGHT - 30);

    s32 textY = (h / 2) - 6;
    s32 leftAlign = (-(w / 2)) + 7;

    Gfx *bg = menu_create_chaos_text_bg(STATS_TOTALS_X, STATS_TOTALS_Y, w, h, 217);

    create_dl_translation_matrix(&gDisplayListHead, MENU_MTX_PUSH, x, y, 0);
    gSPDisplayList(gDisplayListHead++, bg);

    slowtext_setup_ortho_rendering(FT_FONT_SMALL_BOLD);

    slowtext_draw_ortho_text(0, textY -= STATS_PADDING, "TOTALS", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);

    stats_menu_write_playtime_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Play Time:", gSaveBuffer.menuData.scoreData.totalPlayTime);
    stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Clears:", gSaveBuffer.menuData.scoreData.totalClears, 0x9F, 0x9F, 0x9F);
    stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "New Games:", gSaveBuffer.menuData.scoreData.totalAttempts, 0x9F, 0x9F, 0x9F);
    stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Deaths:", gSaveBuffer.menuData.scoreData.totalDeaths, 0xFF, 0x15, 0x25);
    stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Stars:", gSaveBuffer.menuData.scoreData.totalStars, 0xD0, 0xC4, 0x00);
    stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Blue Stars:", gSaveBuffer.menuData.scoreData.totalBlueStars, 0x47, 0x8D, 0xCE);

    stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Total Patches:", gSaveBuffer.menuData.scoreData.totalPatches, 0x9F, 0x9F, 0x9F);
    stats_menu_write_basic_stat(leftAlign + 15, -leftAlign, textY -= STATS_PADDING, "Positive:", gSaveBuffer.menuData.scoreData.totalPositive, 
                                sEffectColors[EFFECT_COLOR_GOOD][0], sEffectColors[EFFECT_COLOR_GOOD][1], sEffectColors[EFFECT_COLOR_GOOD][2]);
    stats_menu_write_basic_stat(leftAlign + 15, -leftAlign, textY -= STATS_PADDING, "Negative:", gSaveBuffer.menuData.scoreData.totalNegative, 
                                sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2]);

    stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Most Active Patches:", gSaveBuffer.menuData.scoreData.mostActive, 0x9F, 0x9F, 0x9F);

    stats_menu_write_favorite_patch(leftAlign, textY -= STATS_PADDING, w - 30, "Favorite Positive Patch:", gSaveBuffer.menuData.scoreData.favoritePositive,
                                    sEffectColors[EFFECT_COLOR_GOOD][0], sEffectColors[EFFECT_COLOR_GOOD][1], sEffectColors[EFFECT_COLOR_GOOD][2]);

    stats_menu_write_favorite_patch(leftAlign, textY -= (STATS_PADDING * 3), w - 30, "Favorite Negative Patch:", gSaveBuffer.menuData.scoreData.favoriteNegative,
                                    sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2]);

    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_stats_menu_best() {
    s32 x = gChaosStatsMenu.bestX;
    s32 y = STATS_BEST_Y;
    s32 w = (SCREEN_WIDTH / 2) - 15;
    s32 h = ((SCREEN_HEIGHT / 3) * 2) - 15;

    s32 textY = (h / 2) - 6;
    s32 leftAlign = (-(w / 2)) + 7;

    Gfx *bg = menu_create_chaos_text_bg(STATS_BEST_X, STATS_BEST_Y, w, h, 217);

    create_dl_translation_matrix(&gDisplayListHead, MENU_MTX_PUSH, x, y, 0);
    gSPDisplayList(gDisplayListHead++, bg);

    slowtext_setup_ortho_rendering(FT_FONT_SMALL_BOLD);

    slowtext_draw_ortho_text(0, textY -= STATS_PADDING, "BEST CLEAR", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);

    if(!gSaveBuffer.menuData.scoreData.isBestClear) {
        slowtext_draw_ortho_text(0, -STATS_PADDING, "No Clear Data", FT_FLAG_ALIGN_CENTER, 0x7F, 0x7F, 0x7F, 0xFF);
    } else {
        stats_menu_write_difficulty(leftAlign, -leftAlign, textY -= STATS_PADDING, gSaveBuffer.menuData.scoreData.bestDifficulty);

        slowtext_draw_ortho_text(leftAlign, textY -= STATS_PADDING, "Gamemode:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
        switch (gSaveBuffer.menuData.scoreData.bestGameMode) {
            case CHAOS_GAMEMODE_HARDCORE:
                slowtext_draw_ortho_text(-leftAlign, textY, "Hardcore", FT_FLAG_ALIGN_RIGHT, 0xAF, 0x5F, 0xCF, 0xFF);
                break;
            case CHAOS_GAMEMODE_CHALLENGE:
                slowtext_draw_ortho_text(-leftAlign, textY, "Challenge", FT_FLAG_ALIGN_RIGHT, 0xBB, 0xA1, 0x24, 0xFF);
                break;
            case CHAOS_GAMEMODE_CLASSIC:
            default:
                slowtext_draw_ortho_text(-leftAlign, textY, "Classic", FT_FLAG_ALIGN_RIGHT, 0xAF, 0xAF, 0xAF, 0xFF);
                break;
        }

        stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Total Stars:", gSaveBuffer.menuData.scoreData.bestStars, 0xD0, 0xC4, 0x00);
        stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Blue Stars:", gSaveBuffer.menuData.scoreData.bestBlueStars, 0x47, 0x8D, 0xCE);
        stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Total Deaths:", gSaveBuffer.menuData.scoreData.bestDeaths, 0xFF, 0x15, 0x25);
        stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Game Loads:", gSaveBuffer.menuData.scoreData.bestGameLoads, 0x9F, 0x9F, 0x9F);
        stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Total Patches:", gSaveBuffer.menuData.scoreData.bestTotalPatches, 0x9F, 0x9F, 0x9F);
        stats_menu_write_playtime_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Play Time:", gSaveBuffer.menuData.scoreData.bestPlayTime);
    }

    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_stats_menu_hardcore() {
    s32 x = gChaosStatsMenu.hardcoreX;
    s32 y = STATS_HARDCORE_Y;
    s32 w = (SCREEN_WIDTH / 2) - 15;
    s32 h = (SCREEN_HEIGHT / 3) - 35;

    s32 textY = (h / 2) - 6;
    s32 leftAlign = (-(w / 2)) + 7;

    Gfx *bg = menu_create_chaos_text_bg(STATS_HARDCORE_X, STATS_HARDCORE_Y, w, h, 217);
    
    create_dl_translation_matrix(&gDisplayListHead, MENU_MTX_PUSH, x, y, 0);
    gSPDisplayList(gDisplayListHead++, bg);

    slowtext_setup_ortho_rendering(FT_FONT_SMALL_BOLD);

    slowtext_draw_ortho_text(0, textY -= STATS_PADDING, "HARDCORE SCORE", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);

    if(gSaveBuffer.menuData.scoreData.bestHardcoreStars == 0) {
        slowtext_draw_ortho_text(0, -STATS_PADDING, "No Hardcore Data", FT_FLAG_ALIGN_CENTER, 0x7F, 0x7F, 0x7F, 0xFF);
    } else {
        stats_menu_write_difficulty(leftAlign, -leftAlign, textY -= STATS_PADDING, gSaveBuffer.menuData.scoreData.bestHardcoreDifficulty);
        stats_menu_write_basic_stat(leftAlign, -leftAlign, textY -= STATS_PADDING, "Stars:", gSaveBuffer.menuData.scoreData.bestHardcoreStars, 0xD0, 0xC4, 0x00);
    }

    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_stats_conf_delete() {
    s32 x = SCREEN_CENTER_X;
    s32 y = SCREEN_CENTER_Y;

    shade_screen();

    Gfx *bg = menu_create_chaos_text_bg(x, y, 298, 70, 255);
    create_dl_translation_matrix(&gDisplayListHead, MENU_MTX_PUSH, x, y, 0);
    create_dl_scale_matrix(&gDisplayListHead, MENU_MTX_NOPUSH, gChaosStatsMenu.confScale, 1.0f, 1.0f);
    gSPDisplayList(gDisplayListHead++, bg);

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(0, 0, "Are you sure you want to delete all saved statistics?", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(-30, -20, "Yes", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(30, -20, "No", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering();

    f32 xPos = (gChaosStatsMenu.menu.index) ? 10 : -50;
    Gfx *cursor = menu_create_cursor(xPos, -10, 0.75f, 0xFF, 0xFF, 0xFF, 0xFF);
    gSPDisplayList(gDisplayListHead++, cursor);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_stats_button_prompts() {
    if(gChaosStatsMenu.menu.flags & CHAOS_STATS_HALT_INPUT) return;

    struct ButtonPromptList prompts = {0};

    switch(gChaosStatsMenu.menu.menuState) {
        case STATS_STATE_DEFAULT:
            menu_add_button_prompt(&prompts, MENU_PROMPT_START_BUTTON, "Delete Data (Hold)");
            menu_add_button_prompt(&prompts, MENU_PROMPT_B_BUTTON, "Back");
            break;
        case STATS_STATE_CONF_DELETE:
            menu_add_button_prompt(&prompts, MENU_PROMPT_A_BUTTON, "Select");
            menu_add_button_prompt(&prompts, MENU_PROMPT_B_BUTTON, "Back");
            break;
    }

    menu_render_button_prompt_list(SCREEN_WIDTH - 35, SCREEN_HEIGHT - 33, &prompts);
}

void render_stats_menu() {
    menu_strip_coverage();

    create_dl_ortho_matrix(&gDisplayListHead);
    squish_ui(&gDisplayListHead);
    
    render_stats_menu_totals();
    render_stats_menu_best();
    render_stats_menu_hardcore();

    if(gChaosStatsMenu.menu.flags & CHAOS_STATS_DRAW_CONF) {
        render_stats_conf_delete();
    }
    
    render_stats_button_prompts();
}
