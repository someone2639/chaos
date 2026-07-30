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
#include "emutest.h"
#include "chaos_settings.h"

#define PATCH_LIST_SIZE     6
#define ACT_DESC_WIDTH      126
struct ChaosPauseMenu sChaosPauseMenu = {.chaosListStart = 0};
struct ChaosPauseMenu *gChaosPauseMenu = &sChaosPauseMenu;
s16 sPrevHudFlags = HUD_DISPLAY_DEFAULT;

/*
    Scrolls the background for the mini patch cards in the active patch menu
*/
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

/*
    Sets the default settings for the active patches menu
*/
void init_active_patches_menu() {
    bzero(gChaosPauseMenu->chaosList, sizeof(struct ChaosActiveEntry *) * CHAOS_PATCH_ENTRIES);
    gChaosPauseMenu->chaosListSize = 0;
    gChaosPauseMenu->chaosListStart = 0;
    gChaosPauseMenu->activePatchesMenu.flags = (ACTIVE_PATCHES_MENU_ACTIVE | ACTIVE_PATCHES_MENU_HALT_INPUT);
    gChaosPauseMenu->activePatchesMenu.index = 0;
    gChaosPauseMenu->activePatchesMenu.animTimer = 0;
    gChaosPauseMenu->activePatchesMenu.animFrames = MENU_ANIM_LOOP;
    gChaosPauseMenu->activePatchesMenu.animId = ACTIVE_PATCHES_MENU_ANIM_STARTUP;
    gChaosPauseMenu->activePatchesMenu.animPhase = 0;
    gChaosPauseMenu->activePatchesMenu.menuState = ACTIVE_PATCHES_MENU_STATE_DEFAULT;
    gChaosPauseMenu->descX = ACTIVE_PATCH_DESC_X_START;
    gChaosPauseMenu->cardX = MINI_CARD_X_START;
    gChaosPauseMenu->extDescScale = 0.0f;
    sPrevHudFlags = gHudDisplay.flags;
}

/*
    Fills the active patches menu with all active patches
*/
void active_patches_menu_fill() {
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        struct ChaosActiveEntry *entry = &gChaosActiveEntries[i];
        active_patches_menu_append(entry);
    }
}

/* 
    Appends an active patch to the active patches menu list
*/
void active_patches_menu_append(struct ChaosActiveEntry *entry) {
    gChaosPauseMenu->chaosList[gChaosPauseMenu->chaosListSize] = entry;
    gChaosPauseMenu->chaosListSize++;
}

/*
    Gets the selected patch in the active patches menu
*/
struct ChaosActiveEntry *active_patches_get_selected_patch() {
    return gChaosPauseMenu->chaosList[gChaosPauseMenu->activePatchesMenu.index];
}

/*
    Draws the prompt for removing a patch
*/
void draw_active_patches_conf_remove(Gfx **dl) {
    s32 x = SCREEN_CENTER_X;
    s32 y = SCREEN_CENTER_Y;

    struct ChaosActiveEntry *patch = gChaosPauseMenu->chaosList[gChaosPauseMenu->activePatchesMenu.index];
    const struct ChaosPatch *patchInfo = &gChaosPatches[patch->id];
    char buf[256];

    sprintf(buf, "Remove @FF1525FF%s@--------?", patchInfo->name);

    Gfx *dlHead = *dl;

    Gfx *bg = menu_create_chaos_text_bg(x, y, 298, 70, 255);
    create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, x, y, 0);
    create_dl_scale_matrix(&dlHead, MENU_MTX_NOPUSH, gChaosPauseMenu->confScale, 1.0f, 1.0f);
    gSPDisplayList(dlHead++, bg);

    slowtext_setup_ortho_rendering(&dlHead, FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(&dlHead, 0, 0, buf, FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(&dlHead, -30, -20, "Yes", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(&dlHead, 30, -20, "No", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering(&dlHead);

    f32 xPos = (gChaosPauseMenu->confIndex) ? 10 : -50;
    Gfx *cursor = menu_create_cursor(xPos, -10, 0.75f, 0xFF, 0xFF, 0xFF, 0xFF);
    gSPDisplayList(dlHead++, cursor);

    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    *dl = dlHead;
}

/*
    Draws the extended description for the selected patch in the active patches menu
*/
void draw_active_patch_ext_desc(Gfx **dl, struct ChaosActiveEntry *patch) {
    const struct ChaosPatch *patchInfo = &gChaosPatches[patch->id];
    const char *patchDesc = patchInfo->longDescription;
    f32 scale = gChaosPauseMenu->extDescScale;

    u8 effectR = sEffectColors[patchInfo->effectType][0];
    u8 effectG = sEffectColors[patchInfo->effectType][1];
    u8 effectB = sEffectColors[patchInfo->effectType][2];

    Gfx *dlHead = *dl;

    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, SCREEN_CENTER_X, SCREEN_CENTER_Y, 0);
    gSPMatrix(dlHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    guScale(scaleMtx, 1.0f, scale, 1.0f);
    gSPMatrix(dlHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    Gfx *bg = menu_create_chaos_text_bg(SCREEN_CENTER_X, SCREEN_CENTER_Y, 298, 218, 255);
    gSPDisplayList(dlHead++, bg);

    slowtext_setup_ortho_rendering(&dlHead, FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text_linebreaks(&dlHead, -142, 87, DESC_STRING_WIDTH, patchDesc, FT_FLAG_ALIGN_LEFT, 
        effectR, effectG, effectB, 0xFF);
    slowtext_finished_rendering(&dlHead);

    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    *dl = dlHead;
}

/*
    Draws a default message for when there are no active patches
*/
void draw_default_patch_desc(Gfx **dl, f32 x, f32 y) {
    Gfx *dlHead = *dl;

    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(dlHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    Gfx *bg = menu_create_chaos_text_bg(ACTIVE_PATCH_DESC_X, ACTIVE_PATCH_DESC_Y, 136, 152, 217);
    gSPDisplayList(dlHead++, bg);

    slowtext_setup_ortho_rendering(&dlHead, FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text_linebreaks(&dlHead, -62, 55, ACT_DESC_WIDTH, "Erm... why don't you play the game first, buddy?", FT_FLAG_ALIGN_LEFT, 
        0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_setup_ortho_rendering(&dlHead, FT_FONT_OUTLINE);
    slowtext_draw_ortho_text_linebreaks(&dlHead, -62, 110, ACT_DESC_WIDTH, "No Active Patches!", FT_FLAG_ALIGN_LEFT, 
        0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering(&dlHead);

    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    *dl = dlHead;
}

/*
    Draws the patch quality beads
*/
static void draw_patch_quality(Gfx **dl, s32 quality) {
    Gfx *dlHead = *dl;

    gDPSetPrimColor(dlHead++, 0, 0, 
                    sQualityColors[quality][0], sQualityColors[quality][1], sQualityColors[quality][2], 255);

    Mtx *transMtx = alloc_display_list(sizeof(Mtx) * 2);
    gSPDisplayList(dlHead++, patch_quality_bead_begin);
    guTranslate(transMtx, (s32) (55 + ((13.0f / 2.0f) * (quality - 1))), 83, 0);
    gSPMatrix(dlHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    for(int i = 0; i < quality; i++) {
        guTranslate(transMtx, -13, 0, 0);
        gSPMatrix(dlHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gSPDisplayList(dlHead++, patch_quality_bead);
    }
    gSPDisplayList(dlHead++, patch_quality_bead_end);
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    *dl = dlHead;
}

/*
    Draws the patch description for the selected patch in the active patches menu
*/
void draw_active_patch_desc(Gfx **dl, f32 x, f32 y, struct ChaosActiveEntry *patch) {
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

    Gfx *dlHead = *dl;

    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(dlHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    Gfx *bg = menu_create_chaos_text_bg(ACTIVE_PATCH_DESC_X, ACTIVE_PATCH_DESC_Y, 136, 152, 217);
    gSPDisplayList(dlHead++, bg);

    draw_patch_quality(&dlHead, patchInfo->severity);

    slowtext_setup_ortho_rendering(&dlHead, FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text_linebreaks(&dlHead, -62, 55, ACT_DESC_WIDTH, patchDesc, FT_FLAG_ALIGN_LEFT, 
        effectR, effectG, effectB, 0xFF);
    slowtext_setup_ortho_rendering(&dlHead, FT_FONT_OUTLINE);
    slowtext_draw_ortho_text_linebreaks(&dlHead, -62, 110, ACT_DESC_WIDTH, patchName, FT_FLAG_ALIGN_LEFT, 
        0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text_linebreaks(&dlHead, -62, 75, CARD_STRING_WIDTH, durationString, FT_FLAG_ALIGN_LEFT, 
        0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering(&dlHead);

    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    *dl = dlHead;
}

/*
    Draws a mini patch card at the specified x/y coords
*/
void draw_mini_patch_card(Gfx **dl, f32 x, f32 y, struct ChaosActiveEntry *patch) {
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
    assert_args(length < ARRAY_COUNT(drawName) - 1, "draw_mini_patch_card:\nPatch name too long:\n%s", patchInfo->name);
    nameY = (lines == 1) ? -10 : -3;
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));

    Gfx *dlHead = *dl;

    gDPSetPrimColor(dlHead++, 0, 0, effectR, effectG, effectB, 0xFF);
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(dlHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(dlHead++, patch_bg_s_mesh_s_mesh);

    if(patchInfo->durationType == CHAOS_DURATION_STARS || patchInfo->durationType == CHAOS_DURATION_USE_COUNT) {
        sprintf(timerText, "%d", patch->remainingDuration);
        gSPDisplayList(dlHead++, patch_use_type_start);
        draw_patch_type(&dlHead, 42, 0, patchInfo->durationType);
        gSPDisplayList(dlHead++, patch_use_type_end);
    } else if (patchInfo->durationType == CHAOS_DURATION_INFINITE) {
        sprintf(timerText, "`"); // Infinity symbol
        gSPDisplayList(dlHead++, patch_use_type_start);
        draw_patch_type(&dlHead, 42, 0, patchInfo->durationType);
        gSPDisplayList(dlHead++, patch_use_type_end);
    } else {
        timerText[0] = '\0';
    }

    slowtext_setup_ortho_rendering(&dlHead, FT_FONT_SMALL_THIN);
    slowtext_draw_ortho_text(&dlHead, -58, nameY, drawName, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_setup_ortho_rendering(&dlHead, FT_FONT_OUTLINE);
    slowtext_draw_ortho_text(&dlHead, 50, -10, timerText, FT_FLAG_ALIGN_LEFT, 0xD0, 0xC4, 0x00, 0xFF);
    slowtext_finished_rendering(&dlHead);
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    *dl = dlHead;
}

/*
    Main rendering function for the active patches menu
*/
void render_active_patches(Gfx **dl) {
    Gfx *dlHead = *dl;

    create_dl_ortho_matrix(&dlHead);
    shade_screen(&dlHead);
    scroll_mini_patch_cards();
    squish_ui(&dlHead);

    //Draw default message if no patches available
    if (gChaosPauseMenu->chaosListSize == 0) {
        draw_default_patch_desc(&dlHead, gChaosPauseMenu->descX, ACTIVE_PATCH_DESC_Y);
        *dl = dlHead;
        return;
    }

    f32 cardX = gChaosPauseMenu->cardX;
    f32 descX = gChaosPauseMenu->descX;
    s32 selection = gChaosPauseMenu->activePatchesMenu.index;
    s32 numPatches = gChaosPauseMenu->chaosListSize;
    s32 listStart = gChaosPauseMenu->chaosListStart;
    s32 listEnd = (numPatches > PATCH_LIST_SIZE) ? (listStart + PATCH_LIST_SIZE) : numPatches;
    s32 triangleAlpha = ((gGlobalTimer % 50) < 25) ? 0xFF : 0x00;

    for(int i = listStart; i < listEnd; i++) {
        draw_mini_patch_card(&dlHead, cardX + (20 * (i == selection)), SCREEN_HEIGHT - 32 - (35 * (i - listStart)), gChaosPauseMenu->chaosList[i]);
    }

    draw_active_patch_desc(&dlHead, descX, ACTIVE_PATCH_DESC_Y, gChaosPauseMenu->chaosList[selection]);

    if(gChaosPauseMenu->activePatchesMenu.flags & ACTIVE_PATCHES_MENU_DRAW_EXT_DESC) {
        draw_active_patch_ext_desc(&dlHead, gChaosPauseMenu->chaosList[selection]);
    }

    if(gChaosPauseMenu->activePatchesMenu.menuState != ACTIVE_PATCHES_MENU_STATE_SHOW_EXT_DESC) {
        //Draw the menu scroll indicator arrows
        gDPSetEnvColor(dlHead++, 255, 255, 255, triangleAlpha);

        if(listStart != 0) {
            create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, cardX + 8, SCREEN_HEIGHT - 14, 0);
            create_dl_scale_matrix(&dlHead, MENU_MTX_NOPUSH, 0.8f, 0.8f, 1.0f);
            create_dl_rotation_matrix(&dlHead, MENU_MTX_NOPUSH, 90.0f, 0, 0, 1.0f);
            gSPDisplayList(dlHead++, dl_draw_triangle);
            gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
        }

        if(listEnd < numPatches) {
            create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, cardX - 8, 15, 0);
            create_dl_scale_matrix(&dlHead, MENU_MTX_NOPUSH, 0.8f, 0.8f, 1.0f);
            create_dl_rotation_matrix(&dlHead, MENU_MTX_NOPUSH, 270.0f, 0, 0, 1.0f);
            gSPDisplayList(dlHead++, dl_draw_triangle);
            gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
        }
    }

    if(gChaosPauseMenu->activePatchesMenu.flags & ACTIVE_PATCHES_MENU_DRAW_CONF) {
        draw_active_patches_conf_remove(&dlHead);
    }

    if(!(gChaosPauseMenu->activePatchesMenu.flags & ACTIVE_PATCHES_MENU_HALT_INPUT)) {
        render_active_patches_menu_button_prompts(&dlHead);
    }

    *dl = dlHead;
}

/*
    Updates the bounds for the displayed patches in the active patches menu
    based on the currently selected index. Called whenever the index is updated 
    to ensure that the menu scrolls correctly.
*/
void update_active_patch_list_bounds() {
    s32 selection = gChaosPauseMenu->activePatchesMenu.index;
    s32 start = gChaosPauseMenu->chaosListStart;
    s32 numPatches = gChaosPauseMenu->chaosListSize;
    s32 end = start + PATCH_LIST_SIZE;
    if(PATCH_LIST_SIZE >= numPatches) {
        return;
    }

    if(selection == 0) {
        gChaosPauseMenu->chaosListStart = 0;
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

/*
    Button prompts for the active patches menu
*/
void render_active_patches_menu_button_prompts(Gfx **dl) {
    s32 selection = gChaosPauseMenu->activePatchesMenu.index;
    struct ChaosActiveEntry *patch = gChaosPauseMenu->chaosList[selection];
    const struct ChaosPatch *patchInfo = &gChaosPatches[patch->id];
    struct ButtonPromptList prompts = {0};
    s32 yPos = PAUSE_BUTTON_PROMPTS_Y;

    Gfx *dlHead = *dl;

    switch(gChaosPauseMenu->activePatchesMenu.menuState) {
        case ACTIVE_PATCHES_MENU_STATE_DEFAULT:
            menu_add_button_prompt(&prompts, MENU_PROMPT_B_BUTTON, "Back");
            if(patchInfo->longDescription){
                const s32 color1[4] = {0x7F, 0x7F, 0x7F, -1};
                const s32 color2[4] = {0xFF, 0xFF, 0xFF, -1};
                char str[32];

                menu_apply_color_oscillation_to_string(str, "Details", 22, color1, color2);
                menu_add_button_prompt(&prompts, MENU_PROMPT_Z_TRIG, str);
            }
            break;
        case ACTIVE_PATCHES_MENU_STATE_SHOW_EXT_DESC:
            menu_add_button_prompt(&prompts, MENU_PROMPT_B_BUTTON, "Back");
            yPos -= 7;
            break;
        case ACTIVE_PATCHES_MENU_STATE_ELIMINATION:
            menu_add_button_prompt(&prompts, MENU_PROMPT_A_BUTTON, "Remove Patch");
            if(patchInfo->longDescription){
                const s32 color1[4] = {0x7F, 0x7F, 0x7F, -1};
                const s32 color2[4] = {0xFF, 0xFF, 0xFF, -1};
                char str[32];

                menu_apply_color_oscillation_to_string(str, "Details", 22, color1, color2);
                menu_add_button_prompt(&prompts, MENU_PROMPT_Z_TRIG, str);
            }
            break;
        case ACTIVE_PATCHES_MENU_STATE_CONF_ELIMINATION:
            menu_add_button_prompt(&prompts, MENU_PROMPT_A_BUTTON, "Confirm");
            menu_add_button_prompt(&prompts, MENU_PROMPT_B_BUTTON, "Back");
            break;
    }

    menu_render_button_prompt_list(&dlHead, SCREEN_WIDTH - 33, yPos, &prompts);

    *dl = dlHead;
}

/*
    Handles input for the active patches menu's default state
*/
void handle_active_patches_inputs_state_default() {
    s32 listEmpty = (gChaosPauseMenu->chaosListSize == 0);
    s32 selection = gChaosPauseMenu->activePatchesMenu.index;

    if(gPlayer1Controller->buttonPressed & (R_TRIG | B_BUTTON | A_BUTTON | START_BUTTON)) {
        menu_play_anim(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_ANIM_ENDING);
        gPlayer1Controller->buttonPressed &= ~R_TRIG;
    } else if((gPlayer1Controller->buttonPressed & (L_TRIG | Z_TRIG)) && !listEmpty) {
        struct ChaosActiveEntry *patch = gChaosPauseMenu->chaosList[selection];
        const struct ChaosPatch *patchInfo = &gChaosPatches[patch->id];
        //Only transition to extended description state if an extended description exists
        if(patchInfo->longDescription) {
            gChaosPauseMenu->prevState = gChaosPauseMenu->activePatchesMenu.menuState;
            menu_play_anim(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_ANIM_EXT_DESC_APPEAR);
            menu_set_state(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_STATE_SHOW_EXT_DESC);
            play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
        } else {
            play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
        }
    } else if(!listEmpty && menu_navigate_vertical(&selection, 0, gChaosPauseMenu->chaosListSize, TRUE)) {
        play_sound(SOUND_MENU_MESSAGE_NEXT_PAGE, gGlobalSoundSource);
        gChaosPauseMenu->activePatchesMenu.index = selection;
        update_active_patch_list_bounds();
    }
}

/*
    Handles input for the active patches menu's extended description state
*/
void handle_active_patches_inputs_state_show_ext_desc() {
    if(gPlayer1Controller->buttonPressed & (L_TRIG | Z_TRIG | B_BUTTON | A_BUTTON | START_BUTTON)) {
        menu_play_anim(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_ANIM_EXT_DESC_DISAPPEAR);
        menu_set_state(&gChaosPauseMenu->activePatchesMenu, gChaosPauseMenu->prevState);
    }
}

/*
    Handles input for the elimination mode state
*/
void handle_active_patches_inputs_state_elimination() {
    s32 selection = gChaosPauseMenu->activePatchesMenu.index;

    if(gPlayer1Controller->buttonPressed & A_BUTTON) {
        menu_set_state(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_STATE_CONF_ELIMINATION);
        menu_play_anim(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_ANIM_CONF_APPEAR);
        play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
        gChaosPauseMenu->confIndex = 0;
    } else if((gPlayer1Controller->buttonPressed & (L_TRIG | Z_TRIG))) {
        struct ChaosActiveEntry *patch = gChaosPauseMenu->chaosList[selection];
        const struct ChaosPatch *patchInfo = &gChaosPatches[patch->id];
        //Only transition to extended description state if an extended description exists
        if(patchInfo->longDescription) {
            gChaosPauseMenu->prevState = gChaosPauseMenu->activePatchesMenu.menuState;
            menu_play_anim(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_ANIM_EXT_DESC_APPEAR);
            menu_set_state(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_STATE_SHOW_EXT_DESC);
            play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
        } else {
            play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
        }
    } else if(menu_navigate_vertical(&selection, 0, gChaosPauseMenu->chaosListSize, TRUE)) {
        play_sound(SOUND_MENU_MESSAGE_NEXT_PAGE, gGlobalSoundSource);
        gChaosPauseMenu->activePatchesMenu.index = selection;
        update_active_patch_list_bounds();
    }
}

/*
    Handles input for the confirmation dialogue for elimination mode
*/
void handle_active_patches_inputs_state_conf_elimination() {
    s32 selection = gChaosPauseMenu->confIndex;

    if((gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) && (selection == 0)) {
        menu_play_anim(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_ANIM_CONF_DISAPPEAR_2);
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
    } else if((gPlayer1Controller->buttonPressed & B_BUTTON) || ((gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)) && (selection == 1))) {
        menu_set_state(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_STATE_ELIMINATION);
        menu_play_anim(&gChaosPauseMenu->activePatchesMenu, ACTIVE_PATCHES_MENU_ANIM_CONF_DISAPPEAR_1);
        play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
    } else if(menu_navigate_horizontal(&selection, 0, 2, TRUE)) {
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
    }

    gChaosPauseMenu->confIndex = selection;
}

/*
    Main input handling function for the active patches menu
*/
void handle_active_patches_inputs() {    
    switch(gChaosPauseMenu->activePatchesMenu.menuState) {
        case ACTIVE_PATCHES_MENU_STATE_DEFAULT:
            handle_active_patches_inputs_state_default();
            break;
        case ACTIVE_PATCHES_MENU_STATE_SHOW_EXT_DESC:
            handle_active_patches_inputs_state_show_ext_desc();
            break;
        case ACTIVE_PATCHES_MENU_STATE_ELIMINATION:
            handle_active_patches_inputs_state_elimination();
            break;
        case ACTIVE_PATCHES_MENU_STATE_CONF_ELIMINATION:
            handle_active_patches_inputs_state_conf_elimination();
            break;
    }
}

/*
    Startup animation for the active patches menu
*/
#define ACTIVE_PATCH_MENU_START_FRAMES      7
s32 active_patches_menu_anim_startup() {
    struct ChaosMenu *menu = &gChaosPauseMenu->activePatchesMenu;

    if(menu->animPhase == 0) {
        gHudDisplay.flags = HUD_DISPLAY_NONE;
        menu->flags |= ACTIVE_PATCHES_MENU_HALT_INPUT;
        menu->animFrames = ACTIVE_PATCH_MENU_START_FRAMES;
        
        s32 animTimer = menu->animTimer;
        f32 prog = ((f32)animTimer / (f32)menu->animFrames);

        gChaosPauseMenu->descX = menu_anim_s32(prog, MENU_EASE_OUT, ACTIVE_PATCH_DESC_X_START, ACTIVE_PATCH_DESC_X);
        gChaosPauseMenu->cardX = menu_anim_s32(prog, MENU_EASE_OUT, MINI_CARD_X_START, MINI_CARD_X);

        if(animTimer == 0) {
            play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
        }
    } else {
        menu->flags &= ~ACTIVE_PATCHES_MENU_HALT_INPUT;
        menu->animFrames = MENU_ANIM_LOOP;
    }
    
    return FALSE;
}

/*
    Ending animation for the active patches menu
*/
#define ACTIVE_PATCH_MENU_END_FRAMES  7
s32 active_patches_menu_anim_ending() {
    struct ChaosMenu *menu = &gChaosPauseMenu->activePatchesMenu;

    if(menu->animPhase == 0) {
        menu->flags |= ACTIVE_PATCHES_MENU_HALT_INPUT;
        menu->animFrames = ACTIVE_PATCH_MENU_END_FRAMES;

        s32 animTimer = menu->animTimer;
        f32 prog = ((f32)animTimer / (f32)menu->animFrames);

        gChaosPauseMenu->descX = menu_anim_s32(prog, MENU_EASE_IN, ACTIVE_PATCH_DESC_X, ACTIVE_PATCH_DESC_X_START);
        gChaosPauseMenu->cardX = menu_anim_s32(prog, MENU_EASE_IN, MINI_CARD_X, MINI_CARD_X_START);

        if(animTimer == 0) {
            play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
        }
    } else {
        menu->flags &= ~ACTIVE_PATCHES_MENU_ACTIVE;
        menu->animFrames = MENU_ANIM_LOOP;
        gHudDisplay.flags = sPrevHudFlags;
    }
    return FALSE;
}

/*
    Extended description appear animation for the active patches menu
*/
#define ACTIVE_PATCHES_MENU_EXT_DESC_APPEAR_ANIM_FRAMES   7
s32 active_patches_menu_anim_ext_desc_appear() {
    struct ChaosMenu *menu = &gChaosPauseMenu->activePatchesMenu;

    if(menu->animPhase == 0) {
        menu->animFrames = ACTIVE_PATCHES_MENU_EXT_DESC_APPEAR_ANIM_FRAMES;
        menu->flags |= ACTIVE_PATCHES_MENU_HALT_INPUT;
        menu->flags |= ACTIVE_PATCHES_MENU_DRAW_EXT_DESC;

        s32 animTimer = menu->animTimer;
        f32 prog = ((f32)animTimer / (f32)menu->animFrames);

        gChaosPauseMenu->extDescScale = menu_anim_f32(prog, MENU_EASE_NONE, 0.0f, 1.0f);

        return FALSE;
    } else {
        menu->flags &= ~ACTIVE_PATCHES_MENU_HALT_INPUT;
        return TRUE;
    }
}

/*
    Extended description dissapear animation for the active patches menu
*/
#define ACTIVE_PATCHES_MENU_EXT_DESC_DISAPPEAR_ANIM_FRAMES   7
s32 active_patches_menu_anim_ext_desc_disappear() {
    struct ChaosMenu *menu = &gChaosPauseMenu->activePatchesMenu;

    if(menu->animPhase == 0) {
        menu->animFrames = ACTIVE_PATCHES_MENU_EXT_DESC_DISAPPEAR_ANIM_FRAMES;
        menu->flags |= ACTIVE_PATCHES_MENU_HALT_INPUT;

        s32 animTimer = menu->animTimer;
        f32 prog = ((f32)animTimer / (f32)menu->animFrames);

        gChaosPauseMenu->extDescScale = menu_anim_f32(prog, MENU_EASE_NONE, 1.0f, 0.0f);

        if(animTimer == 0) {
            play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
        }
        return FALSE;
    } else {
        menu->flags &= ~ACTIVE_PATCHES_MENU_DRAW_EXT_DESC;
        menu->flags &= ~ACTIVE_PATCHES_MENU_HALT_INPUT;
        return TRUE;
    }
}

#define ACTIVE_PATCHES_ANIM_CONF_APPEAR_FRAMES   7
s32 active_patches_menu_anim_conf_appear() {
    struct ChaosMenu *menu = &gChaosPauseMenu->activePatchesMenu;
    if(menu->animPhase == 0) {
        menu->animFrames = ACTIVE_PATCHES_ANIM_CONF_APPEAR_FRAMES;
        menu->flags |= (ACTIVE_PATCHES_MENU_HALT_INPUT | ACTIVE_PATCHES_MENU_DRAW_CONF);

        f32 prog = menu_get_anim_prog(menu);

        gChaosPauseMenu->confScale = menu_anim_f32(prog, MENU_EASE_OUT, 0.0f, 1.0f);
    } else {
        menu->flags &= ~ACTIVE_PATCHES_MENU_HALT_INPUT;
        menu->animFrames = MENU_ANIM_LOOP;
    }
    return FALSE;
}

#define ACTIVE_PATCHES_ANIM_CONF_DISAPPEAR_FRAMES   7
s32 active_patches_menu_anim_conf_disappear_1() {
    struct ChaosMenu *menu = &gChaosPauseMenu->activePatchesMenu;
    if(menu->animPhase == 0) {
        menu->animFrames = ACTIVE_PATCHES_ANIM_CONF_DISAPPEAR_FRAMES;
        menu->flags |= ACTIVE_PATCHES_MENU_HALT_INPUT;

        f32 prog = menu_get_anim_prog(menu);

        gChaosPauseMenu->confScale = menu_anim_f32(prog, MENU_EASE_IN, 1.0f, 0.0f);
    } else {
        menu->flags &= ~(ACTIVE_PATCHES_MENU_DRAW_CONF | ACTIVE_PATCHES_MENU_HALT_INPUT);
    }
    return FALSE;
}

// Same as the above, but also transitions into the ending animation
s32 active_patches_menu_anim_conf_disappear_2() {
    struct ChaosMenu *menu = &gChaosPauseMenu->activePatchesMenu;
    if(menu->animPhase == 0) {
        menu->animFrames = ACTIVE_PATCHES_ANIM_CONF_DISAPPEAR_FRAMES;
        menu->flags |= ACTIVE_PATCHES_MENU_HALT_INPUT;

        f32 prog = menu_get_anim_prog(menu);

        gChaosPauseMenu->confScale = menu_anim_f32(prog, MENU_EASE_IN, 1.0f, 0.0f);
    } else {
        menu->flags &= ~ACTIVE_PATCHES_MENU_DRAW_CONF;
        menu_play_anim(menu, ACTIVE_PATCHES_MENU_ANIM_ENDING);
    }
    return FALSE;
}

/*
    Active patches menu animations
*/
s32 (*sActivePatchesMenuAnims[])(void) = {
    &active_patches_menu_anim_startup,
    &active_patches_menu_anim_ending,
    &active_patches_menu_anim_ext_desc_appear,
    &active_patches_menu_anim_ext_desc_disappear,
    &active_patches_menu_anim_conf_appear,
    &active_patches_menu_anim_conf_disappear_1,
    &active_patches_menu_anim_conf_disappear_2,
};

/*
    Main update function for the active patches menu
*/
void update_active_patches_menu() {
    if((gChaosPauseMenu->chaosListStart >= gChaosPauseMenu->chaosListSize) || (gChaosPauseMenu->activePatchesMenu.index >= gChaosPauseMenu->chaosListSize)) {
        gChaosPauseMenu->chaosListStart = 0;
        gChaosPauseMenu->activePatchesMenu.index = 0;
    }

    if(!(gChaosPauseMenu->activePatchesMenu.flags & ACTIVE_PATCHES_MENU_HALT_INPUT)) {
        handle_active_patches_inputs();
    }

    menu_update_anims(&gChaosPauseMenu->activePatchesMenu, sActivePatchesMenuAnims);
}

/*
    VANILLA PAUSE MENU
*/

/*
    Button prompts for the vanilla pause screen
*/
void render_pause_screen_button_prompts() {
    menu_single_button_prompt(&gDisplayListHead, SCREEN_WIDTH - 32, PAUSE_BUTTON_PROMPTS_Y, MENU_PROMPT_R_TRIG, "Active Patches", FALSE);
    menu_single_button_prompt(&gDisplayListHead, SCREEN_WIDTH - 144, PAUSE_BUTTON_PROMPTS_Y, MENU_PROMPT_Z_TRIG, "Message Log", FALSE);
    menu_single_button_prompt(&gDisplayListHead, 15, PAUSE_BUTTON_PROMPTS_Y, MENU_PROMPT_L_TRIG, "Settings", TRUE);
}