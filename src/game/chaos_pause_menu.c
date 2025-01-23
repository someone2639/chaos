#include <ultra64.h>
#include <PR/gbi.h>

#include "segment2.h"
#include "print.h"
#include "gfx_dimensions.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "object_helpers.h"
#include "fasttext.h"
#include "level_update.h"
#include "debug.h"
#include "src/engine/math_util.h"
#include "audio/external.h"
#include "sm64.h"
#include "save_file.h"
#include "chaos_menus.h"
#include "chaos_pause_menu.h"
#include "main.h"
#include "chaos/chaos.h"
#include "patch_selection_ui.h"
#include "buffers/framebuffers.h"
#include "emutest.h"

#define PATCH_LIST_SIZE     6
#define ACT_DESC_WIDTH      126
struct ChaosPauseMenu sChaosPauseMenu = {.chaosListStart = 0};
struct ChaosPauseMenu *gChaosPauseMenu = &sChaosPauseMenu;
u16 sPauseScreenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

/*
    Draws a copy of the pause screen as the background. Improves performance on console.
    Mostly copied from the crash screen lol
*/
void draw_pause_screen_framebuffer_bg() {
    Texture *image = (Texture * )sPauseScreenBuffer;
    s32 width = SCREEN_WIDTH;
    s32 height = SCREEN_HEIGHT;
    s32 posW, posH, imW, imH;
    s32 i     = 0;
    s32 num   = 256;
    s32 maskW = 1;
    s32 maskH = 1;

    gDPPipeSync(gDisplayListHead++);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);

    gDPSetCycleType( gDisplayListHead++, G_CYC_COPY);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);

    // Find how best to seperate the horizontal. Keep going until it finds a whole value.
    while (TRUE) {
        f32 val = (f32)width / (f32)num;

        if ((s32)val == val && (s32) val >= 1) {
            imW = num;
            break;
        }
        num /= 2;
        if (num == 1) {
            return;
        }
    }
    // Find the tile height
    imH = 64 / (imW / 32); // This gets the vertical amount.

    num = 2;
    // Find the width mask
    while (TRUE) {
        if ((s32) num == imW) {
            break;
        }
        num *= 2;
        maskW++;
        if (maskW == 9) {
            return;
        }
    }
    num = 2;
    // Find the height mask
    while (TRUE) {
        if ((s32) num == imH) {
            break;
        }
        num *= 2;
        maskH++;
        if (maskH == 9) {
            return;
        }
    }
    num = height;
    // Find the height remainder
    s32 peakH  = height - (height % imH);
    s32 cycles = (width * peakH) / (imW * imH);

    // Pass 1
    for (i = 0; i < cycles; i++) {
        posW = 0;
        posH = i * imH;
        while (posH >= peakH) {
            posW += imW;
            posH -= peakH;
        }

        gDPLoadSync(gDisplayListHead++);
        gDPLoadTextureTile(gDisplayListHead++,
            image, G_IM_FMT_RGBA, G_IM_SIZ_16b, width, height, posW, posH, ((posW + imW) - 1), ((posH + imH) - 1), 0, (G_TX_NOMIRROR | G_TX_WRAP), (G_TX_NOMIRROR | G_TX_WRAP), maskW, maskH, 0, 0);
        gSPScisTextureRectangle(gDisplayListHead++,
            ((posW) << 2),
            ((posH) << 2),
            (((posW + imW) - 1) << 2),
            (((posH + imH) - 1) << 2),
            G_TX_RENDERTILE, 0, 0, (4 << 10), (1 << 10));
    }
    // If there's a remainder on the vertical side, then it will cycle through that too.
    if (height-peakH != 0) {
        posW = 0;
        posH = peakH;
        for (i = 0; i < (width / imW); i++) {
            posW = i * imW;
            gDPLoadSync(gDisplayListHead++);
            gDPLoadTextureTile(gDisplayListHead++,
                image, G_IM_FMT_RGBA, G_IM_SIZ_16b, width, height, posW, posH, ((posW + imW) - 1), (height - 1), 0, (G_TX_NOMIRROR | G_TX_WRAP), (G_TX_NOMIRROR | G_TX_WRAP), maskW, maskH, 0, 0);
            gSPScisTextureRectangle(gDisplayListHead++,
                (posW) << 2,
                (posH) << 2,
                ((posW + imW) - 1) << 2,
                ((posH + imH) - 1) << 2,
                G_TX_RENDERTILE, 0, 0, (4 << 10), (1 << 10));
        }
    }
}

void scroll_mini_patch_cards() {
	int i = 0;
	int count = 4;
	int width = 128 * 0x20;
	int height = 64 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(patch_bg_s_mesh_s_mesh_vtx_0);

	deltaX = (int)(-0.25f * 0x20) % width;
	deltaY = (int)(-0.25f * 0x20) % height;

	if (absi(currentX) > width) {
		deltaX -= (int)(absi(currentX) / width) * width * signum_positive(deltaX);
	}
	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[0] += deltaX;
		vertices[i].n.tc[1] += deltaY;
	}
	currentX += deltaX;	currentY += deltaY;
}

void scroll_settings_panel() {
    int i = 0;
	int count = 4;
	int width = 64 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(opt_bg_opt_mesh_mesh_vtx_0);

	deltaX = (int)(0.1 * 0x20) % width;
	deltaY = (int)(0.1 * 0x20) % height;

	if (absi(currentX) > width) {
		deltaX -= (int)(absi(currentX) / width) * width * signum_positive(deltaX);
	}
	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[0] += deltaX;
		vertices[i].n.tc[1] += deltaY;
	}
	currentX += deltaX;	currentY += deltaY;
}

void scroll_act_desc_bg() {
    int i = 0;
	int count = 4;
	int width = 64 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(act_desc_bg_act_desc_mesh_mesh_vtx_0);
	Vtx *ext_vertices = segmented_to_virtual(ext_desc_bg_ext_mesh_mesh_vtx_0);

	deltaX = (int)(0.1 * 0x20) % width;
	deltaY = (int)(0.1 * 0x20) % height;

	if (absi(currentX) > width) {
		deltaX -= (int)(absi(currentX) / width) * width * signum_positive(deltaX);
	}
	if (absi(currentY) > height) {
		deltaY -= (int)(absi(currentY) / height) * height * signum_positive(deltaY);
	}

	for (i = 0; i < count; i++) {
		vertices[i].n.tc[0] += deltaX;
		vertices[i].n.tc[1] += deltaY;
		ext_vertices[i].n.tc[0] += deltaX;
		ext_vertices[i].n.tc[1] += deltaY;
	}
	currentX += deltaX;	currentY += deltaY;
}

void init_active_patches_menu() {
    gChaosPauseMenu->activePatchesMenu.flags = (ACTIVE_PATCHES_MENU_ACTIVE | ACTIVE_PATCHES_MENU_HALT_INPUT);
    gChaosPauseMenu->activePatchesMenu.animTimer = 0;
    gChaosPauseMenu->activePatchesMenu.animFrames = MENU_ANIM_LOOP;
    gChaosPauseMenu->activePatchesMenu.animId = ACTIVE_PATCHES_MENU_ANIM_STARTUP;
    gChaosPauseMenu->activePatchesMenu.animPhase = 0;
    gChaosPauseMenu->activePatchesMenu.menuState = ACTIVE_PATCHES_MENU_STATE_DEFAULT;
    gChaosPauseMenu->descX = ACTIVE_PATCH_DESC_X_START;
    gChaosPauseMenu->cardX = MINI_CARD_X_START;
    gChaosPauseMenu->extDescScale = 0.0f;
}

void draw_active_patch_ext_desc(struct ChaosActiveEntry *patch) {
    const struct ChaosPatch *patchInfo = &gChaosPatches[patch->id];
    const char *patchDesc = patchInfo->longDescription;
    f32 scale = gChaosPauseMenu->extDescScale;

    u8 effectR = sEffectColors[patchInfo->effectType][0];
    u8 effectG = sEffectColors[patchInfo->effectType][1];
    u8 effectB = sEffectColors[patchInfo->effectType][2];

    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, SCREEN_CENTER_X, SCREEN_CENTER_Y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    guScale(scaleMtx, 1.0f, scale, 1.0f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gSPDisplayList(gDisplayListHead++, ext_desc_bg_ext_mesh_mesh);

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text_linebreaks(-142, 87, DESC_STRING_WIDTH, patchDesc, FT_FLAG_ALIGN_LEFT, 
        effectR, effectG, effectB, 0xFF);
    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void draw_active_patch_desc(f32 x, f32 y, struct ChaosActiveEntry *patch) {
    const struct ChaosPatch *patchInfo = &gChaosPatches[patch->id];
    const char *patchName = patchInfo->name;
    const char *patchDesc = patchInfo->shortDescription;
    char durationString[16];

    u8 effectR = sEffectColors[patchInfo->effectType][0];
    u8 effectG = sEffectColors[patchInfo->effectType][1];
    u8 effectB = sEffectColors[patchInfo->effectType][2];

    switch(patchInfo->durationType) {
        case CHAOS_DURATION_STARS:
            sprintf(durationString, "Time Left: %d", patch->remainingDuration);
            break;
        case CHAOS_DURATION_USE_COUNT:
            sprintf(durationString, "Uses Left: %d", patch->remainingDuration);
            break;
        case CHAOS_DURATION_INFINITE:
            sprintf(durationString, "Time Left: `");
            break;
        default:
            durationString[0] = '\0';
            break;
    }


    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, act_desc_bg_act_desc_mesh_mesh);

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text_linebreaks(-62, 55, ACT_DESC_WIDTH, patchDesc, FT_FLAG_ALIGN_LEFT, 
        effectR, effectG, effectB, 0xFF);
    slowtext_setup_ortho_rendering(FT_FONT_OUTLINE);
    slowtext_draw_ortho_text_linebreaks(-62, 110, CARD_STRING_WIDTH, patchName, FT_FLAG_ALIGN_LEFT, 
        0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text_linebreaks(-62, 75, CARD_STRING_WIDTH, durationString, FT_FLAG_ALIGN_LEFT, 
        0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void draw_mini_patch_card(f32 x, f32 y, struct ChaosActiveEntry *patch) {
    const struct ChaosPatch *patchInfo = &gChaosPatches[patch->id];
    const char* patchName = patchInfo->name;
    char drawName[48];
    char timerText[4];
    f32 nameY;
    s32 lines, length;
    
    u8 effectR = sEffectColors[patchInfo->effectType][0];
    u8 effectG = sEffectColors[patchInfo->effectType][1];
    u8 effectB = sEffectColors[patchInfo->effectType][2];

    //Center name if it's only one line long
    fasttext_compute_print_text_with_line_breaks(FT_FONT_SMALL_THIN, CARD_STRING_WIDTH, &lines, &length, drawName, patchName);
    nameY = (lines == 1) ? -10 : -3;
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));

    gDPSetPrimColor(gDisplayListHead++, 0, 0, effectR, effectG, effectB, 0xFF);
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, patch_bg_s_mesh_s_mesh);

    if(patchInfo->durationType == CHAOS_DURATION_STARS || patchInfo->durationType == CHAOS_DURATION_USE_COUNT) {
        sprintf(timerText, "%d", patch->remainingDuration);
        gSPDisplayList(gDisplayListHead++, patch_use_type_start);
        draw_patch_type(42, 0, patchInfo->durationType);
        gSPDisplayList(gDisplayListHead++, patch_use_type_end);
    } else if (patchInfo->durationType == CHAOS_DURATION_INFINITE) {
        sprintf(timerText, "`"); // Infinity symbol
        gSPDisplayList(gDisplayListHead++, patch_use_type_start);
        draw_patch_type(42, 0, patchInfo->durationType);
        gSPDisplayList(gDisplayListHead++, patch_use_type_end);
    } else {
        timerText[0] = '\0';
    }

    slowtext_setup_ortho_rendering(FT_FONT_SMALL_THIN);
    slowtext_draw_ortho_text(-58, nameY, drawName, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_setup_ortho_rendering(FT_FONT_OUTLINE);
    slowtext_draw_ortho_text(50, -10, timerText, FT_FLAG_ALIGN_LEFT, 0xD0, 0xC4, 0x00, 0xFF);
    slowtext_finished_rendering();
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void copy_pause_screen_framebuffer() {
    if(gIsConsole) {
        gChaosPauseMenu->activePatchesMenu.flags |= ACTIVE_PATCHES_MENU_STOP_GAME_RENDER;
        bcopy(gFramebuffers[sRenderingFramebuffer], sPauseScreenBuffer, sizeof(gFramebuffers[0]));
    }
}

void render_active_patches() {
    if(gChaosPauseMenu->activePatchesMenu.flags & ACTIVE_PATCHES_MENU_STOP_GAME_RENDER) {
        draw_pause_screen_framebuffer_bg();
    } else {
        shade_screen();
    }

    if (gChaosActiveEntryCount == NULL || *gChaosActiveEntryCount == 0) {
        return;
    }

    scroll_mini_patch_cards();
    scroll_act_desc_bg();

    f32 cardX = gChaosPauseMenu->cardX;
    f32 descX = gChaosPauseMenu->descX;
    s32 selection = gChaosPauseMenu->activePatchesMenu.selectedMenuIndex;
    s32 numPatches = *gChaosActiveEntryCount;
    s32 listStart = gChaosPauseMenu->chaosListStart;
    s32 listEnd = (numPatches > PATCH_LIST_SIZE) ? (listStart + PATCH_LIST_SIZE) : numPatches;

    for(int i = listStart; i < listEnd; i++) {
        draw_mini_patch_card(cardX + (20 * (i == selection)), SCREEN_HEIGHT - 35 - (35 * (i - listStart)), &gChaosActiveEntries[i]);
    }

    draw_active_patch_desc(descX, ACTIVE_PATCH_DESC_Y, &gChaosActiveEntries[selection]);

    if(gChaosPauseMenu->activePatchesMenu.flags & ACTIVE_PATCHES_MENU_DRAW_EXT_DESC) {
        draw_active_patch_ext_desc(&gChaosActiveEntries[selection]);
    }
}

void update_active_patch_list_bounds() {
    s32 selection = gChaosPauseMenu->activePatchesMenu.selectedMenuIndex;
    s32 start = gChaosPauseMenu->chaosListStart;
    s32 numPatches = *gChaosActiveEntryCount;
    s32 end = start + PATCH_LIST_SIZE;
    if(PATCH_LIST_SIZE >= numPatches) {
        return;
    }

    if(selection > end - 2) {
        start = selection - PATCH_LIST_SIZE + 2;
    } else if (selection < start + 1) {
        start = selection - 1;
    }

    end = start + PATCH_LIST_SIZE;

    if(end > numPatches) {
        start = numPatches - PATCH_LIST_SIZE;
    } else if (start < 0) {
        start = 0;
    }

    gChaosPauseMenu->chaosListStart = start;

}

void render_settings_panel() {
    scroll_settings_panel();

    Mtx *transMtx = alloc_display_list(sizeof(Mtx) * 2);
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    f32 x = 42;
    f32 y = 52;

    u8 aspRatCol43;
    u8 aspRatCol169;
    u8 musColOn;
    u8 musColOff;
    s32 cursorX = (gChaosPauseMenu->settingsMenu.selectedMenuIndex % 2) ? 0 : -30;
    s32 cursorY = (gChaosPauseMenu->settingsMenu.selectedMenuIndex < 2) ? 5 : -35;

    if(gConfig.widescreen) {
        aspRatCol43 = 0x7F;
        aspRatCol169 = 0xFF;
    } else {
        aspRatCol43 = 0xFF;
        aspRatCol169 = 0x7F;
    }

    if(gConfig.disableBGMusic) {
        musColOn = 0x7F;
        musColOff = 0xFF;
    } else {
        musColOn = 0xFF;
        musColOff = 0x7F;
    }

    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, opt_bg_opt_mesh_mesh);

    slowtext_setup_ortho_rendering(FT_FONT_SMALL_BOLD);
    slowtext_draw_ortho_text(0, 25, "Aspect\nRatio:", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(0, -25, "Music:", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_setup_ortho_rendering(FT_FONT_SMALL_THIN);
    slowtext_draw_ortho_text(-15, 0, "4:3", FT_FLAG_ALIGN_CENTER, aspRatCol43, aspRatCol43, aspRatCol43, 0xFF);
    slowtext_draw_ortho_text(15, 0, "16:9", FT_FLAG_ALIGN_CENTER, aspRatCol169, aspRatCol169, aspRatCol169, 0xFF);
    slowtext_draw_ortho_text(-15, -40, "On", FT_FLAG_ALIGN_CENTER, musColOn, musColOn, musColOn, 0xFF);
    slowtext_draw_ortho_text(15, -40, "Off", FT_FLAG_ALIGN_CENTER, musColOff, musColOff, musColOff, 0xFF);
    slowtext_finished_rendering();

    //Cursor
    guTranslate(transMtx, cursorX, cursorY, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    guScale(scaleMtx, 0.5f, 0.5f, 1.0f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gDPSetEnvColor(gDisplayListHead++, 0xFF, 0xFF, 0xFF, 0xFF);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void handle_active_patches_inputs_state_default(u32 stickDir) {
    s32 prevSelection = gChaosPauseMenu->activePatchesMenu.selectedMenuIndex;
    s32 selection = prevSelection;

    if(gPlayer1Controller->buttonPressed & (R_TRIG | B_BUTTON | A_BUTTON | START_BUTTON)) {
        menu_play_anim(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_ANIM_ENDING);
        gChaosPauseMenu->activePatchesMenu.flags &= ~ACTIVE_PATCHES_MENU_STOP_GAME_RENDER;
        gPlayer1Controller->buttonPressed &= ~R_TRIG;
    } else if(gPlayer1Controller->buttonPressed & (L_TRIG | Z_TRIG)) {
        struct ChaosActiveEntry *patch = &gChaosActiveEntries[selection];
        const struct ChaosPatch *patchInfo = &gChaosPatches[patch->id];
        if(patchInfo->longDescription) {
            menu_play_anim(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_ANIM_EXT_DESC_APPEAR);
            menu_set_state(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_STATE_SHOW_EXT_DESC);
            play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
        } else {
            play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
        }
    } else if(gPlayer1Controller->buttonPressed & U_JPAD || stickDir & MENU_JOYSTICK_DIR_UP) {
        selection--;
    } else if (gPlayer1Controller->buttonPressed & D_JPAD || stickDir & MENU_JOYSTICK_DIR_DOWN) {
        selection++;
    }

    if(selection < 0) {
        selection = *gChaosActiveEntryCount - 1;
    } else if (selection >= *gChaosActiveEntryCount) {
        selection = 0;
    }

    if(selection != prevSelection) {
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
        gChaosPauseMenu->activePatchesMenu.selectedMenuIndex = selection;
        update_active_patch_list_bounds();
    }
}

void handle_active_patches_inputs_state_show_ext_desc() {
    if(gPlayer1Controller->buttonPressed & (L_TRIG | Z_TRIG | B_BUTTON | A_BUTTON | START_BUTTON)) {
        menu_play_anim(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_ANIM_EXT_DESC_DISAPPEAR);
        menu_set_state(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_STATE_DEFAULT);
    }
}

void handle_active_patches_inputs() {
    if(!gChaosActiveEntryCount) {
        return;
    }

    u32 stickDir = menu_update_joystick_dir(&gChaosPauseMenu->activePatchesMenu);
    
    switch(gChaosPauseMenu->activePatchesMenu.menuState) {
        case ACTIVE_PATCHES_MENU_STATE_DEFAULT:
            handle_active_patches_inputs_state_default(stickDir);
            break;
        case ACTIVE_PATCHES_MENU_STATE_SHOW_EXT_DESC:
            handle_active_patches_inputs_state_show_ext_desc();
            break;
    }
}

#define ACTIVE_PATCH_MENU_WAIT_FB_FRAMES    3
#define ACTIVE_PATCH_MENU_START_FRAMES      10
s32 active_patches_menu_anim_startup() {
    s32 phase = gChaosPauseMenu->activePatchesMenu.animPhase;
    s32 animTimer = gChaosPauseMenu->activePatchesMenu.animTimer;
    f32 animPercent;
    switch(phase) {
        case 0:
            gChaosPauseMenu->activePatchesMenu.animFrames = ACTIVE_PATCH_MENU_WAIT_FB_FRAMES;
            if(animTimer == ACTIVE_PATCH_MENU_WAIT_FB_FRAMES) {
                copy_pause_screen_framebuffer();
            }
            break;
        case 1:
            gChaosPauseMenu->activePatchesMenu.flags |= ACTIVE_PATCHES_MENU_HALT_INPUT;
            gChaosPauseMenu->activePatchesMenu.animFrames = ACTIVE_PATCH_MENU_START_FRAMES;
            animPercent = sins((0x3FFF / gChaosPauseMenu->activePatchesMenu.animFrames) * animTimer);
            gChaosPauseMenu->descX = menu_translate_percentage(ACTIVE_PATCH_DESC_X_START, ACTIVE_PATCH_DESC_X, animPercent);
            gChaosPauseMenu->cardX = menu_translate_percentage(MINI_CARD_X_START, MINI_CARD_X, animPercent);
            break;
        default:
            gChaosPauseMenu->activePatchesMenu.flags &= ~ACTIVE_PATCHES_MENU_HALT_INPUT;
            gChaosPauseMenu->activePatchesMenu.animFrames = MENU_ANIM_LOOP;
            break;
    }
    
    return FALSE;
}

#define ACTIVE_PATCH_MENU_END_FRAMES  10
s32 active_patches_menu_anim_ending() {
    s32 phase = gChaosPauseMenu->activePatchesMenu.animPhase;
    s32 animTimer = gChaosPauseMenu->activePatchesMenu.animTimer;
    f32 animPercent;
    if(!phase) {
        gChaosPauseMenu->activePatchesMenu.flags |= ACTIVE_PATCHES_MENU_HALT_INPUT;
        gChaosPauseMenu->activePatchesMenu.animFrames = ACTIVE_PATCH_MENU_END_FRAMES;
        animPercent = 1.0f - coss((0x3FFF / gChaosPauseMenu->activePatchesMenu.animFrames) * animTimer);
        gChaosPauseMenu->descX = menu_translate_percentage(ACTIVE_PATCH_DESC_X, ACTIVE_PATCH_DESC_X_START, animPercent);
        gChaosPauseMenu->cardX = menu_translate_percentage(MINI_CARD_X, MINI_CARD_X_START, animPercent);
    } else {
        gChaosPauseMenu->activePatchesMenu.flags &= ~ACTIVE_PATCHES_MENU_ACTIVE;
        gChaosPauseMenu->activePatchesMenu.animFrames = MENU_ANIM_LOOP;
    }
    return FALSE;
}

#define ACTIVE_PATCHES_MENU_EXT_DESC_APPEAR_ANIM_FRAMES   7
s32 active_patches_menu_anim_ext_desc_appear() {
    s32 animTimer = gChaosPauseMenu->activePatchesMenu.animTimer;
    f32 animPhase = gChaosPauseMenu->activePatchesMenu.animPhase;

    if(animPhase) {
        gChaosPauseMenu->activePatchesMenu.flags &= ~ACTIVE_PATCHES_MENU_HALT_INPUT;
        return TRUE;
    }

    gChaosPauseMenu->activePatchesMenu.animFrames = ACTIVE_PATCHES_MENU_EXT_DESC_APPEAR_ANIM_FRAMES;
    f32 animPercent = (1.0f / gChaosPauseMenu->activePatchesMenu.animFrames) * animTimer;
    gChaosPauseMenu->activePatchesMenu.flags |= ACTIVE_PATCHES_MENU_HALT_INPUT;

    gChaosPauseMenu->activePatchesMenu.flags |= ACTIVE_PATCHES_MENU_DRAW_EXT_DESC;

    f32 scale = menu_translate_percentage(0.0f, 1.0f, animPercent);
    gChaosPauseMenu->extDescScale = scale;

    return FALSE;
}

#define ACTIVE_PATCHES_MENU_EXT_DESC_DISAPPEAR_ANIM_FRAMES   7
s32 active_patches_menu_anim_ext_desc_disappear() {
    s32 animTimer = gChaosPauseMenu->activePatchesMenu.animTimer;
    f32 animPhase = gChaosPauseMenu->activePatchesMenu.animPhase;

    if(animPhase) {
        gChaosPauseMenu->activePatchesMenu.flags &= ~ACTIVE_PATCHES_MENU_DRAW_EXT_DESC;
        gChaosPauseMenu->activePatchesMenu.flags &= ~ACTIVE_PATCHES_MENU_HALT_INPUT;
        return TRUE;
    }

    gChaosPauseMenu->activePatchesMenu.animFrames = ACTIVE_PATCHES_MENU_EXT_DESC_DISAPPEAR_ANIM_FRAMES;
    f32 animPercent = (1.0f / gChaosPauseMenu->activePatchesMenu.animFrames) * animTimer;
    gChaosPauseMenu->activePatchesMenu.flags |= ACTIVE_PATCHES_MENU_HALT_INPUT;

    f32 scale = menu_translate_percentage(1.0f, 0.0f, animPercent);
    gChaosPauseMenu->extDescScale = scale;

    return FALSE;
}

s32 (*sActivePatchesMenuAnims[])(void) = {
    &active_patches_menu_anim_startup,
    &active_patches_menu_anim_ending,
    &active_patches_menu_anim_ext_desc_appear,
    &active_patches_menu_anim_ext_desc_disappear,
};

void update_active_patches_menu() {
    if(!(gChaosPauseMenu->activePatchesMenu.flags & ACTIVE_PATCHES_MENU_HALT_INPUT)) {
        handle_active_patches_inputs();
    }

    menu_update_anims(&gChaosPauseMenu->activePatchesMenu, sActivePatchesMenuAnims);
}

void handle_settings_inputs() {
    u32 stickDir = menu_update_joystick_dir(&gChaosPauseMenu->settingsMenu);
    s32 prevSelection = gChaosPauseMenu->settingsMenu.selectedMenuIndex;
    s32 selection = prevSelection;
    s32 pressedUpDown = ((gPlayer1Controller->buttonPressed & (D_JPAD | U_JPAD)) || stickDir & (MENU_JOYSTICK_DIR_DOWN | MENU_JOYSTICK_DIR_UP));
    s32 pressedLeftRight = ((gPlayer1Controller->buttonPressed & (L_JPAD | R_JPAD)) || stickDir & (MENU_JOYSTICK_DIR_LEFT | MENU_JOYSTICK_DIR_RIGHT));

    if(gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) {
        switch(selection) {
            case 0:
                gConfig.widescreen = 0;
                break;
            case 1:
                gConfig.widescreen = 1;
                break;
            case 2:
                gConfig.disableBGMusic = 0;
                break;
            case 3:
                gConfig.disableBGMusic = 1;
                break;
        }
        save_file_set_widescreen_mode(gConfig.widescreen);
        save_file_set_bg_music(gConfig.disableBGMusic);
    } else if (gPlayer1Controller->buttonPressed & (B_BUTTON | L_TRIG)) {
        gChaosPauseMenu->settingsMenu.flags &= ~CHAOS_SETTINGS_ACTIVE;
        gPlayer1Controller->buttonPressed &= ~(B_BUTTON | L_TRIG);
    } else if(pressedUpDown) {
        selection += 2;
        if(selection > 3) {
            selection = prevSelection - 2;
        }
    } else if (pressedLeftRight) {
        selection++;
        if(selection > 3 || !(selection % 2)) {
            selection = prevSelection - 1;
        }
    }

    if(selection != prevSelection) {
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
        gChaosPauseMenu->settingsMenu.selectedMenuIndex = selection;
    }
}