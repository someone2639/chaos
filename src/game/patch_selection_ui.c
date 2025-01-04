#include <ultra64.h>
#include <PR/gbi.h>

#include "patch_selection_ui.h"
#include "segment2.h"
#include "print.h"
#include "gfx_dimensions.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "object_helpers.h"

//temp
const char testNameGood[] = {"Good Effect"};
const char testNameBad[] = {"Bad Effect"};
const char testDescGood[] = {"Good effect description"};
const char testDescBad[] = {"Bad effect description"};

struct PatchCard sTestChaosPatch1 = {2, 0, testNameGood, testNameBad, testDescGood, testDescBad};
struct PatchCard sTestChaosPatch2 = {3, 2, testNameGood, testNameBad, testDescGood, testDescBad};
struct PatchCard sTestChaosPatch3 = {1, 10, testNameGood, testNameBad, testDescGood, testDescBad};
struct PatchCard sTestChaosPatch4 = {4, 3, testNameGood, testNameBad, testDescGood, testDescBad};

u8 sQualityColors[][4] = {
    {0x9A, 0x9A, 0x9A},
    {0x20, 0xDB, 0x1D},
    {0x47, 0x42, 0xDB},
    {0x8B, 0x00, 0xC5},
};

struct PatchCard *sAvailablePatches[MAX_CARDS] = {
    &sTestChaosPatch1,
    &sTestChaosPatch2,
    &sTestChaosPatch3,
    &sTestChaosPatch4,
};

struct PatchSelectionMenu gPatchSelectionMenu = {
    0, FALSE, 0,
};

void handle_patch_selection_inputs() {
    s32 previousSelection = gPatchSelectionMenu.selectedPatch;

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

    if(gPlayer1Controller->buttonPressed & D_JPAD || (stickY < -60)) {
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

void desc_bg_scroll() {
	int i = 0;
	int count = 4;
	int width = 32 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(desc_bg_mesh_mesh_vtx_0);

	deltaX = (int)(0.5 * 0x20) % width;
	deltaY = (int)(-0.5 * 0x20) % height;

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
    Draws a patch card at the given x, y coordinates
*/
void render_patch_card(f32 x, f32 y, f32 scale, struct PatchCard *card, s32 reverse) {
    s32 quality = card->quality;
    s32 colorID = quality - 1;
    Mtx *cardScaleMtx = alloc_display_list(sizeof(Mtx));
    Mtx *cardTransMtx = alloc_display_list(sizeof(Mtx));
    Mtx *qualityTransMtx = alloc_display_list(sizeof(Mtx) * 2);
    Mtx* timerTransMtx = alloc_display_list(sizeof(Mtx));

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
    gSPDisplayList(gDisplayListHead++, patch_quality_bead_begin);
    guTranslate(qualityTransMtx, -70, 28, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(qualityTransMtx++),
          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    for(int i = 0; i < quality; i++) {
        guTranslate(qualityTransMtx, 13, 0, 0);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(qualityTransMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gSPDisplayList(gDisplayListHead++, patch_quality_bead);
    }
    gSPDisplayList(gDisplayListHead++, patch_quality_bead_end);

    //Draw star timer
    if(card->duration > 0) {
        gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(cardTransMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
        if(scale != 1.0f) {
            gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(cardScaleMtx),
                  G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        }
        guTranslate(timerTransMtx, 37, 25, 0);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(timerTransMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gSPDisplayList(gDisplayListHead++, star_timer_Mesh_mesh);
        //temp
        print_text_fmt_int(x + 42, y + 18, "%d" ,card->duration);
    }

    //temp
    print_text(x - 67, y + 5, card->patchName1);
    print_text(x - 67, y - 15, card->patchName2);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/*
    Draws the patch description at the given x/y coordinates
*/
void render_patch_desc(f32 x, f32 y) {
    s32 selected = gPatchSelectionMenu.selectedPatch;
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, desc_bg_mesh_mesh);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    //temp
    print_text(14, 60, sAvailablePatches[selected]->patchDesc1);
    print_text(14, 30, sAvailablePatches[selected]->patchDesc2);
}

void render_patch_selection_cursor(f32 x, f32 y) {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    guTranslate(transMtx, x + 65, y - 30, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, hand_Mesh_mesh);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

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

    patch_bg_scroll();
    desc_bg_scroll();
    create_dl_ortho_matrix();

    //Ugly and hopefully temporary
    render_patch_card(CARD_X_LEFT, CARD_Y_TOP, cardScale1, sAvailablePatches[0], FALSE);
    render_patch_card(CARD_X_RIGHT, CARD_Y_TOP, cardScale2, sAvailablePatches[1], TRUE);
    render_patch_card(CARD_X_LEFT, CARD_Y_BOTTOM, cardScale3, sAvailablePatches[2], FALSE);
    render_patch_card(CARD_X_RIGHT, CARD_Y_BOTTOM, cardScale4, sAvailablePatches[3], TRUE);
    render_patch_desc(PATCH_DESC_X, PATCH_DESC_Y);
    render_patch_selection_cursor(cursorX, cursorY);
}