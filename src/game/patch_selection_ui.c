#include <ultra64.h>
#include <PR/gbi.h>

#include "patch_selection_ui.h"
#include "segment2.h"
#include "print.h"
#include "gfx_dimensions.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "object_helpers.h"
#include "fasttext.h"
#include "debug.h"

u8 sQualityColors[][3] = {
    {0x20, 0xDB, 0x1D},
    {0x47, 0x42, 0xDB},
    {0x8B, 0x00, 0xC5},
};

u8 sEffectColors[][3] = {
    {0x15, 0xD0, 0x25}, //Good
    {0xC0, 0x25, 0x35}, //Bad
};

struct PatchCard gAvailablePatches[MAX_CARDS];

struct PatchSelectionMenu gPatchSelectionMenu = {
    0, 0, FALSE, 0, PATCH_SELECT_STATE_SELECT,
};

#ifdef DEBUG_PATCH_SELECT_MENU
const char testNameGood[] = {"Good Effect"};
const char testNameBad[] = {"Bad Effect"};
const char testDescGood[] = {"Good effect description"};
const char testDescBad[] = {"Bad effect description"};
const char testExtendedDesc[] = {"This is the extended description\nThere is more information here."};
#endif

/*
    Initializes the patch selection menu
*/
void init_patch_selection_menu() {
#ifdef DEBUG_PATCH_SELECT_MENU
    load_patch_card(0, 2, 0, 1, CARD_TYPE_NONE, CARD_TYPE_TIMED, testNameGood, testNameBad, testDescGood, testDescBad, testExtendedDesc, testExtendedDesc);
    load_patch_card(1, 3, 2, 4, CARD_TYPE_LIMITED_USE, CARD_TYPE_LIMITED_USE, testNameGood, testNameBad, testDescGood, testDescBad, testExtendedDesc, NULL);
    load_patch_card(2, 1, 10, 20, CARD_TYPE_TIMED, CARD_TYPE_LIMITED_USE, testNameGood, testNameBad, testDescGood, testDescBad, NULL, testExtendedDesc);
    load_patch_card(3, 3, 3, 16, CARD_TYPE_LIMITED_USE, CARD_TYPE_TIMED, testNameGood, testNameBad, testDescGood, testDescBad, NULL, NULL);
#else
    load_patch_card(0, 0, 0, 0, CARD_TYPE_NONE, CARD_TYPE_NONE, NULL, NULL, NULL, NULL, NULL, NULL);
    load_patch_card(0, 0, 0, 0, CARD_TYPE_NONE, CARD_TYPE_NONE, NULL, NULL, NULL, NULL, NULL, NULL);
    load_patch_card(0, 0, 0, 0, CARD_TYPE_NONE, CARD_TYPE_NONE, NULL, NULL, NULL, NULL, NULL, NULL);
    load_patch_card(0, 0, 0, 0, CARD_TYPE_NONE, CARD_TYPE_NONE, NULL, NULL, NULL, NULL, NULL, NULL);
#endif
}

/*
    Loads a patch card into gAvailablePatches in the slot specified by index
*/
void load_patch_card(s32 index, s32 quality, s32 effect1DurationOrUses, s32 effect2DurationOrUses, 
        s32 effect1UseType, s32 effect2UseType, const char *effect1Name, const char *effect2Name, 
        const char *effect1Desc, const char *effect2Desc, const char* effect1ExtendedDesc, const char *effect2ExtendedDesc) 
{
    assert(index < MAX_CARDS && index >= 0, "Tried to load a card into an invalid index!");
    gAvailablePatches[index].quality = quality;
    gAvailablePatches[index].patchDurationOrUses1 = effect1DurationOrUses;
    gAvailablePatches[index].patchDurationOrUses2 = effect2DurationOrUses;
    gAvailablePatches[index].patchUseType1 = effect1UseType;
    gAvailablePatches[index].patchUseType2 = effect2UseType;
    gAvailablePatches[index].patchName1 = effect1Name;
    gAvailablePatches[index].patchName2 = effect2Name;
    gAvailablePatches[index].patchDesc1 = effect1Desc;
    gAvailablePatches[index].patchDesc2 = effect2Desc;
    gAvailablePatches[index].extendedDesc1 = effect1ExtendedDesc;
    gAvailablePatches[index].extendedDesc2 = effect2ExtendedDesc;
    //TODO: Add an id or reference of some sort to an actual patch so it can be applied after selection
}

/*
    Resets the values of the patch selection menu to their defaults. Should always be called after the patch selection menu is exited.
*/
void reset_patch_selection_menu() {
    gPatchSelectionMenu.selectedPatch = 0;
    gPatchSelectionMenu.selectedMenuIndex = 0;
    gPatchSelectionMenu.isActive = FALSE;
    gPatchSelectionMenu.framesSinceLastStickInput = 0;
    gPatchSelectionMenu.menuState = PATCH_SELECT_STATE_SELECT;
}

/*
    Handles the player inputs for the patch selection state in the patch selection menu
*/
void handle_inputs_PATCH_SELECT_STATE_SELECT(f32 stickX, f32 stickY) {
    s32 previousSelection = gPatchSelectionMenu.selectedPatch;

    if(gPlayer1Controller->buttonPressed & A_BUTTON || gPlayer1Controller->buttonPressed & START_BUTTON) {
        gPatchSelectionMenu.menuState = PATCH_SELECT_STATE_CONFIRMATION;
    } else if (gPlayer1Controller->buttonPressed & Z_TRIG || gPlayer1Controller->buttonPressed & L_TRIG) {
        struct PatchCard *selectedCard = &gAvailablePatches[gPatchSelectionMenu.selectedPatch];
        if(selectedCard->extendedDesc1 || selectedCard->extendedDesc2) {
            gPatchSelectionMenu.menuState = PATCH_SELECT_STATE_SHOW_EXTENDED_DESC;
        }
    } else if(gPlayer1Controller->buttonPressed & D_JPAD || (stickY < -60)) {
        gPatchSelectionMenu.selectedPatch += 2;
        if(gPatchSelectionMenu.selectedPatch > MAX_CARDS - 1) {
            gPatchSelectionMenu.selectedPatch = previousSelection;
        }
    } else if (gPlayer1Controller->buttonPressed & U_JPAD || (stickY > 60)) {
        gPatchSelectionMenu.selectedPatch -= 2;
        if(gPatchSelectionMenu.selectedPatch < 0) {
            gPatchSelectionMenu.selectedPatch = previousSelection;
        }
    } else if(gPlayer1Controller->buttonPressed & R_JPAD || (stickX > 60)) {
        gPatchSelectionMenu.selectedPatch++;
        if(gPatchSelectionMenu.selectedPatch > MAX_CARDS - 1) {
            gPatchSelectionMenu.selectedPatch = previousSelection;
        }
    } else if(gPlayer1Controller->buttonPressed & L_JPAD || (stickX < -60)) {
        gPatchSelectionMenu.selectedPatch--;
        if(gPatchSelectionMenu.selectedPatch < 0) {
            gPatchSelectionMenu.selectedPatch = previousSelection;
        }
    }
}

/*
    Handles the player inputs for the extended description state in the patch selection menu
*/
void handle_inputs_PATCH_SELECT_STATE_SHOW_EXTENDED_DESC() {
    if(gPlayer1Controller->buttonPressed & A_BUTTON || gPlayer1Controller->buttonPressed & START_BUTTON
        || gPlayer1Controller->buttonPressed & B_BUTTON || gPlayer1Controller->buttonPressed & Z_TRIG
        || gPlayer1Controller->buttonPressed & L_TRIG) 
    {
        gPatchSelectionMenu.menuState = PATCH_SELECT_STATE_SELECT;
    }
}

/*
    Handles the player inputs for the confirmation dialog state in the patch selection menu
*/
void handle_inputs_PATCH_SELECT_STATE_CONFIRMATION(f32 stickX, UNUSED f32 stickY) {
    if(gPlayer1Controller->buttonPressed & A_BUTTON || gPlayer1Controller->buttonPressed & START_BUTTON) {
        if(gPatchSelectionMenu.selectedMenuIndex) {
            //No
            gPatchSelectionMenu.menuState = PATCH_SELECT_STATE_SELECT;
            gPatchSelectionMenu.selectedMenuIndex = 0;
        } else {
            //Yes
            gPatchSelectionMenu.menuState = PATCH_SELECT_STATE_CLOSED;
        }
    } else if(gPlayer1Controller->buttonPressed & B_BUTTON) {
        gPatchSelectionMenu.menuState = PATCH_SELECT_STATE_SELECT;
        gPatchSelectionMenu.selectedMenuIndex = 0;
    }
    else if(gPlayer1Controller->buttonPressed & R_JPAD || (stickX > 60)) {
        gPatchSelectionMenu.selectedMenuIndex++;
        if(gPatchSelectionMenu.selectedMenuIndex > 1) {
            gPatchSelectionMenu.selectedMenuIndex = 0;
        }
    } else if(gPlayer1Controller->buttonPressed & L_JPAD || (stickX < -60)) {
        gPatchSelectionMenu.selectedMenuIndex--;
        if(gPatchSelectionMenu.selectedMenuIndex < 0) {
            gPatchSelectionMenu.selectedMenuIndex = 1;
        }
    }
}

/*
    Handles the player inputs for the patch selection menu
*/
void handle_patch_selection_inputs() {
    f32 stickX = gPlayer1Controller->rawStickX;
    f32 stickY = gPlayer1Controller->rawStickY;

    //Prevents the same stick flick from being read on multiple frames
    if((absf(stickX) < 60) && (absf(stickY) < 60)){
        if(gPatchSelectionMenu.framesSinceLastStickInput < 2) {
            gPatchSelectionMenu.framesSinceLastStickInput++;
        }
    } else {
        if(gPatchSelectionMenu.framesSinceLastStickInput < 2) {
            stickX = 0;
            stickY = 0;
        }
        gPatchSelectionMenu.framesSinceLastStickInput = 0;
    }

    switch(gPatchSelectionMenu.menuState) {
        case PATCH_SELECT_STATE_SELECT:
            handle_inputs_PATCH_SELECT_STATE_SELECT(stickX, stickY);
            break;
        case PATCH_SELECT_STATE_CONFIRMATION:
            handle_inputs_PATCH_SELECT_STATE_CONFIRMATION(stickX, stickY);
            break;
        case PATCH_SELECT_STATE_SHOW_EXTENDED_DESC:
            handle_inputs_PATCH_SELECT_STATE_SHOW_EXTENDED_DESC();
            break;
    }
}

/*
    Scrolls the vertex uvs for the patch card background meshes
*/
void patch_bg_scroll() {
    int i = 0;
	int count = 4;
	int width = 128 * 0x20;
	int height = 64 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(patch_bg_mesh_mesh_vtx_0);
	Vtx *vertices_r = segmented_to_virtual(patch_bg_r_mesh_r_mesh_vtx_0);

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
		vertices_r[i].n.tc[0] += deltaX;
		vertices_r[i].n.tc[1] += deltaY;
	}
	currentX += deltaX;	currentY += deltaY;
}

/*
    Scrolls the vertex uvs for the descripton and extended description background meshes
*/
void desc_bg_scroll() {
	int i = 0;
	int count = 4;
	int width = 64 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(desc_bg_mesh_mesh_vtx_0);
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

/*
    Draws the patch quality beads
*/
void draw_patch_quality(s32 quality) {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx) * 2);
    gSPDisplayList(gDisplayListHead++, patch_quality_bead_begin);
    guTranslate(transMtx, -70, 28, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    for(int i = 0; i < quality; i++) {
        guTranslate(transMtx, 13, 0, 0);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gSPDisplayList(gDisplayListHead++, patch_quality_bead);
    }
    gSPDisplayList(gDisplayListHead++, patch_quality_bead_end);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws a patch type indicator at the given x/y coordinates
*/
void draw_patch_type(f32 x, f32 y, s32 type) {
    Mtx* transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    if(type == CARD_TYPE_TIMED) {
        gSPDisplayList(gDisplayListHead++, star_timer);
    } else if (type == CARD_TYPE_LIMITED_USE) {
        gSPDisplayList(gDisplayListHead++, uses_counter);
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws a patch card at the given x/y coordinates
*/
void render_patch_card(f32 x, f32 y, f32 scale, struct PatchCard *card, s32 reverse) {
    s32 quality = card->quality;
    s32 colorID = quality - 1;
    Mtx *cardScaleMtx = alloc_display_list(sizeof(Mtx));
    Mtx *cardTransMtx = alloc_display_list(sizeof(Mtx));
    char timer1Text[4];
    char timer2Text[4];

    gDPSetPrimColor(gDisplayListHead++, 0, 0, 
                    sQualityColors[colorID][0], sQualityColors[colorID][1], sQualityColors[colorID][2], 255);

    //Draw patch bg
    guTranslate(cardTransMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(cardTransMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    if(scale != 1.0f) {
        guScale(cardScaleMtx, scale, scale, 1.0f);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(cardScaleMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    }
    if(reverse) {
        gSPDisplayList(gDisplayListHead++, patch_bg_r_mesh_r_mesh);
    } else {
        gSPDisplayList(gDisplayListHead++, patch_bg_mesh_mesh);
    }
    
    //Draw patch quality beads
    draw_patch_quality(quality);

    //Draw patch type(s)
    gSPDisplayList(gDisplayListHead++, patch_use_type_start);
    if(card->patchDurationOrUses1 > 0) {
        draw_patch_type(42, 15, card->patchUseType1);
        sprintf(timer1Text, "%d", card->patchDurationOrUses1);
    }
    if(card->patchDurationOrUses2 > 0) {
        draw_patch_type(42, -15, card->patchUseType2);
        sprintf(timer2Text, "%d", card->patchDurationOrUses2);
    }
    gSPDisplayList(gDisplayListHead++, patch_use_type_end);

    //Write text
    slowtext_setup_ortho_rendering(FT_FONT_SMALL_THIN);
    slowtext_draw_ortho_text(-63, 5, card->patchName1, FT_FLAG_ALIGN_LEFT, 
        sEffectColors[EFFECT_COLOR_GOOD][0], sEffectColors[EFFECT_COLOR_GOOD][1], sEffectColors[EFFECT_COLOR_GOOD][2], 0xFF);
    slowtext_draw_ortho_text(-63, -25, card->patchName2, FT_FLAG_ALIGN_LEFT, 
        sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2], 0xFF);
    slowtext_setup_ortho_rendering(FT_FONT_OUTLINE);
    if(card->patchDurationOrUses1 > 0) {
        slowtext_draw_ortho_text(51, 6, timer1Text, FT_FLAG_ALIGN_LEFT, 0xD0, 0xC4, 0x00, 0xFF);
    }if(card->patchDurationOrUses2 > 0) {
        slowtext_draw_ortho_text(51, -24, timer2Text, FT_FLAG_ALIGN_LEFT, 0xD0, 0xC4, 0x00, 0xFF);
    }
    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Writes the patch description
*/
void render_patch_desc() {
    s32 selected = gPatchSelectionMenu.selectedPatch;
    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(-142, 15, gAvailablePatches[selected].patchDesc1, FT_FLAG_ALIGN_LEFT, 
        sEffectColors[EFFECT_COLOR_GOOD][0], sEffectColors[EFFECT_COLOR_GOOD][1], sEffectColors[EFFECT_COLOR_GOOD][2], 0xFF);
    slowtext_draw_ortho_text(-142, -15, gAvailablePatches[selected].patchDesc2, FT_FLAG_ALIGN_LEFT, 
        sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2], 0xFF);
    slowtext_finished_rendering();
}

/*
    Draws a confirmation dialog
*/
void render_confirmation_dialog() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    slowtext_draw_ortho_text(0, 0, "Would you like to select this patch?", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(-30, -20, "Yes", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_draw_ortho_text(30, -20, "No", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    slowtext_finished_rendering();

    s32 selected = gPatchSelectionMenu.selectedMenuIndex;
    f32 xPos;

    if(selected) {
        xPos = 10;
    } else {
        xPos = -50;
    }

    guTranslate(transMtx, xPos, -15, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    guScale(scaleMtx, 0.75f, 0.75f, 1.0f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws the patch description at the given x/y coordinates
*/
void render_lower_box(f32 x, f32 y) {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, desc_bg_mesh_mesh);

    switch(gPatchSelectionMenu.menuState) {
        case PATCH_SELECT_STATE_CONFIRMATION:
            render_confirmation_dialog();
            break;
        default:
            render_patch_desc();
            break;
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws the cursor for the patch selection menu at the given x/y coordinates
*/
void render_patch_selection_cursor(f32 x, f32 y) {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, x + 65, y - 30, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, hand_Mesh_mesh);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Shows a dialogue box featuring the extended descriptions of the currently selected card
*/
void render_extended_description () {
    s32 selected = gPatchSelectionMenu.selectedPatch;
    struct PatchCard *selectedCard = &gAvailablePatches[selected];
    
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, SCREEN_CENTER_X, SCREEN_CENTER_Y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, ext_desc_bg_ext_mesh_mesh);

    slowtext_setup_ortho_rendering(FT_FONT_VANILLA_SHADOW);
    if(selectedCard->extendedDesc1) {
        slowtext_draw_ortho_text(-142, 87, selectedCard->extendedDesc1, FT_FLAG_ALIGN_LEFT, 
            sEffectColors[EFFECT_COLOR_GOOD][0], sEffectColors[EFFECT_COLOR_GOOD][1], sEffectColors[EFFECT_COLOR_GOOD][2], 0xFF);
    }
    if(selectedCard->extendedDesc2) {
        //Draw second effect description lower if there are two extended descriptions
        if(selectedCard->extendedDesc1) {
            slowtext_draw_ortho_text(-142, -13, selectedCard->extendedDesc2, FT_FLAG_ALIGN_LEFT, 
                sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2], 0xFF);
        } else {
            slowtext_draw_ortho_text(-142, 87, selectedCard->extendedDesc2, FT_FLAG_ALIGN_LEFT, 
                sEffectColors[EFFECT_COLOR_BAD][0], sEffectColors[EFFECT_COLOR_BAD][1], sEffectColors[EFFECT_COLOR_BAD][2], 0xFF);
        }
    }
    slowtext_finished_rendering();

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Displays the patch selection menu
*/
void display_patch_selection_ui() {
    s32 selectedPatch = gPatchSelectionMenu.selectedPatch;
    f32 cursorX, cursorY;
    f32 cardScale1 = 1.0f, cardScale2 = 1.0f, cardScale3 = 1.0f, cardScale4 = 1.0f;

    switch(selectedPatch) {
        case 0:
            cardScale1 = 1.05f;
            cursorX = CARD_X_LEFT;
            cursorY = CARD_Y_TOP;
            break;
        case 1:
            cardScale2 = 1.05f;
            cursorX = CARD_X_RIGHT;
            cursorY = CARD_Y_TOP;
            break;
        case 2:
            cardScale3 = 1.05f;
            cursorX = CARD_X_LEFT;
            cursorY = CARD_Y_BOTTOM;
            break;
        case 3:
        default:
            cardScale4 = 1.05f;
            cursorX = CARD_X_RIGHT;
            cursorY = CARD_Y_BOTTOM;
            break;
    }

    if(gPatchSelectionMenu.menuState != PATCH_SELECT_STATE_CLOSED) {
        patch_bg_scroll();
        desc_bg_scroll();
        create_dl_ortho_matrix(&gDisplayListHead);

        render_patch_card(CARD_X_LEFT, CARD_Y_TOP, cardScale1, &gAvailablePatches[0], FALSE);
        render_patch_card(CARD_X_RIGHT, CARD_Y_TOP, cardScale2, &gAvailablePatches[1], TRUE);
        render_patch_card(CARD_X_LEFT, CARD_Y_BOTTOM, cardScale3, &gAvailablePatches[2], FALSE);
        render_patch_card(CARD_X_RIGHT, CARD_Y_BOTTOM, cardScale4, &gAvailablePatches[3], TRUE);
        render_lower_box(PATCH_DESC_X, PATCH_DESC_Y);

        if(gPatchSelectionMenu.menuState == PATCH_SELECT_STATE_SHOW_EXTENDED_DESC) {
            render_extended_description();
        } else {
            render_patch_selection_cursor(cursorX, cursorY);
        }
    }
    
}