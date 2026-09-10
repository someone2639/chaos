#include "chaos_stats.h"
#include "save_file.h"
#include "fasttext.h"
#include "audio/external.h"
#include "ingame_menu.h"
#include "chaos_menus.h"
#include "chaos_pause_menu.h"
#include "game_init.h"

static struct ChaosStats sChaosStats;

// Call this before deleting the save/clearing patches
void init_chaos_stats(void) {
    sChaosStats.difficulty = save_file_get_difficulty(gCurrSaveFileNum - 1);
    sChaosStats.gameMode = save_file_get_game_mode(gCurrSaveFileNum - 1);
    sChaosStats.stars = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);
    sChaosStats.blueStars = save_file_get_blue_stars();
    sChaosStats.deaths = save_file_get_death_count();
    sChaosStats.gameLoads = save_file_get_game_loads();
    sChaosStats.totalPatches = save_file_get_total_patches();
    sChaosStats.playTime = save_file_get_play_time();
    sChaosStats.menuTimer = 0;
    bcopy(gChaosActiveEntries, sChaosStats.finalPatches, sizeof(sChaosStats.finalPatches));
    sChaosStats.finalPatchCount = *gChaosActiveEntryCount;
}

void draw_chaos_stats(Gfx **dl) {
    Gfx *dlHead = *dl;
    create_dl_ortho_matrix(&dlHead);

    if(sChaosStats.menuTimer == 0) {
        play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
    }

    f32 scale;
    if(sChaosStats.menuTimer < 7) {
        scale = ((f32)sChaosStats.menuTimer / 7.0f);
    } else {
        scale = 1.0f;
    }

    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));

    guTranslate(transMtx, (SCREEN_WIDTH / 4) + 4, SCREEN_CENTER_Y, 0);
    gSPMatrix(dlHead++, VIRTUAL_TO_PHYSICAL(transMtx),
                    G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    guScale(scaleMtx, 1.0f, scale, 1.0f);
    gSPMatrix(dlHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    Gfx *bg = menu_create_chaos_text_bg((SCREEN_WIDTH / 4) + 4, SCREEN_CENTER_Y, 150, 163, 217);
    gSPDisplayList(dlHead++, bg);
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    fasttext_setup_textrect_rendering(&dlHead, FT_FONT_VANILLA_SHADOW);
    if(sChaosStats.menuTimer > 15) {
        if (!(gChaosPauseMenu->activePatchesMenu.flags & ACTIVE_PATCHES_MENU_ACTIVE)) {
            fasttext_draw_texrect(&dlHead, (SCREEN_WIDTH - 16), (SCREEN_HEIGHT - 24), VERSION_STRING, FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
        }
        fasttext_setup_textrect_rendering(&dlHead, FT_FONT_MEDIUM);
        fasttext_draw_texrect(&dlHead, (SCREEN_WIDTH / 4) + 4, 15, "FINAL SCORE", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
        
        //Difficulty
        fasttext_draw_texrect(&dlHead, 16, 40, "Difficulty:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
        switch(sChaosStats.difficulty) {
            case CHAOS_DIFFICULTY_EASY:
                fasttext_draw_texrect(&dlHead, 150, 40, "Easy", FT_FLAG_ALIGN_RIGHT, 0x05, 0xDf, 0x15, 0xFF);
                break;
            case CHAOS_DIFFICULTY_NORMAL:
            default:
                fasttext_draw_texrect(&dlHead, 150, 40, "Normal", FT_FLAG_ALIGN_RIGHT, 0xAF, 0xAF, 0xAF, 0xFF);
                break;
            case CHAOS_DIFFICULTY_HARD:
                fasttext_draw_texrect(&dlHead, 150, 40, "Hard", FT_FLAG_ALIGN_RIGHT, 0xFF, 0x15, 0x25, 0xFF);
                break;
            case CHAOS_DIFFICULTY_IMPOSSIBLE:
                fasttext_draw_texrect(&dlHead, 150, 40, "Impossible", FT_FLAG_ALIGN_RIGHT, 0x5F, 0x5F, 0x5F, 0xFF);
                break;
        }
    }
    if(sChaosStats.menuTimer > 30) {
        //Gamemode
        fasttext_draw_texrect(&dlHead, 16, 60, "Gamemode:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
        switch (sChaosStats.gameMode) {
            case CHAOS_GAMEMODE_HARDCORE:
                fasttext_draw_texrect(&dlHead, 150, 60, "Hardcore", FT_FLAG_ALIGN_RIGHT, 0xAF, 0x5F, 0xCF, 0xFF);
                break;
            case CHAOS_GAMEMODE_CHALLENGE:
                fasttext_draw_texrect(&dlHead, 150, 60, "Challenge", FT_FLAG_ALIGN_RIGHT, 0xBB, 0xA1, 0x24, 0xFF);
                break;
            case CHAOS_GAMEMODE_CLASSIC:
            default:
                fasttext_draw_texrect(&dlHead, 150, 60, "Classic", FT_FLAG_ALIGN_RIGHT, 0xAF, 0xAF, 0xAF, 0xFF);
                break;
        }
    }
    if(sChaosStats.menuTimer > 45) {
        //Star count
        char starCountText[8];
        s32 starCount = sChaosStats.stars;
        sprintf(starCountText, "%d", starCount);
        fasttext_draw_texrect(&dlHead, 16, 80, "Total Stars:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
        fasttext_draw_texrect(&dlHead, 150, 80, starCountText, FT_FLAG_ALIGN_RIGHT, 0xD0, 0xC4, 0x00, 0xFF);
    }
    if(sChaosStats.menuTimer > 60) {
        //Blue star count
        char blueStarCountText[8];
        s32 blueStarCount = sChaosStats.blueStars;
        sprintf(blueStarCountText, "%d", blueStarCount);
        fasttext_draw_texrect(&dlHead, 16, 100, "Blue Stars:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
        fasttext_draw_texrect(&dlHead, 150, 100, blueStarCountText, FT_FLAG_ALIGN_RIGHT, 0x47, 0x8D, 0xCE, 0xFF);
    }
    if(sChaosStats.menuTimer > 75) {
        //Deaths
        char deathsText[8];
        s32 deathCount = sChaosStats.deaths;
        sprintf(deathsText, "%d", deathCount);
        fasttext_draw_texrect(&dlHead, 16, 120, "Total Deaths:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
        fasttext_draw_texrect(&dlHead, 150, 120, deathsText, FT_FLAG_ALIGN_RIGHT, 0xFF, 0x15, 0x25, 0xFF);
    }
    if(sChaosStats.menuTimer > 90) {
        //Game loads
        char loadsText[8];
        s32 loadCount = sChaosStats.gameLoads;
        sprintf(loadsText, "%d", loadCount);
        fasttext_draw_texrect(&dlHead, 16, 140, "Game Loads:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
        fasttext_draw_texrect(&dlHead, 150, 140, loadsText, FT_FLAG_ALIGN_RIGHT, 0x9F, 0x9F, 0x9F, 0xFF);
    }
    if(sChaosStats.menuTimer > 105) {
        //Total Patches
        char totalPatchesText[8];
        s32 totalPatchesCount = sChaosStats.totalPatches;
        sprintf(totalPatchesText, "%d", totalPatchesCount);
        fasttext_draw_texrect(&dlHead, 16, 160, "Total Patches:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
        fasttext_draw_texrect(&dlHead, 150, 160, totalPatchesText, FT_FLAG_ALIGN_RIGHT, 0x9F, 0x9F, 0x9F, 0xFF);
    }
    if(sChaosStats.menuTimer > 120) {
        //Play time
        u32 playTime = sChaosStats.playTime;
        u32 hours = playTime / (30 * 60 * 60);
        u32 mins = (playTime - (hours * (30 * 60 * 60))) / (60 * 30);
        u32 secs = (playTime - (hours * (30 * 60 * 60)) - (mins * (60 * 30))) / 30;
        char playTimeText[32];
        sprintf(playTimeText, "%dh %dm %ds", hours, mins, secs);
        fasttext_draw_texrect(&dlHead, 16, 180, "Play Time:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
        fasttext_draw_texrect(&dlHead, 150, 180, playTimeText, FT_FLAG_ALIGN_RIGHT, 0x9F, 0x9F, 0x9F, 0xFF);    
    }
    
    fasttext_finished_rendering(&dlHead);
    
    if(!(sChaosStats.menuTimer % 15) && (sChaosStats.menuTimer != 0)) {
        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
    }

    if(sChaosStats.menuTimer > 120) {
        if(gChaosPauseMenu->activePatchesMenu.flags & ACTIVE_PATCHES_MENU_ACTIVE) {
            render_active_patches(&dlHead);
        } else {
            menu_single_button_prompt(&dlHead, 16, SCREEN_HEIGHT - 23, MENU_PROMPT_R_TRIG, "Final Patches", TRUE);

            if(gGoddardVblankCallback) {
                menu_single_button_prompt(&dlHead, 90, SCREEN_HEIGHT - 23, MENU_PROMPT_START_BUTTON, "Continue", TRUE);
            }
        }
    }

    *dl = dlHead;
}

s32 update_chaos_stats(void) {
    if(sChaosStats.menuTimer > 120) {
        if(gChaosPauseMenu->activePatchesMenu.flags & ACTIVE_PATCHES_MENU_ACTIVE) {
            update_active_patches_menu();
        } else {
            if(gPlayer1Controller->buttonPressed & R_TRIG) {
                init_active_patches_menu();
                for(int i = 0; i < sChaosStats.finalPatchCount; i++) {
                    active_patches_menu_append(&sChaosStats.finalPatches[i]);
                }
            } else if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
                return TRUE;
            }
        }
    } else {
        sChaosStats.menuTimer++;
    }

    return FALSE;
}