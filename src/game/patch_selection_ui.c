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
struct PatchCard sTestChaosPatch1 = {2, 0, "Effect 1"};
struct PatchCard sTestChaosPatch2 = {3, 2,  "Effect 2"};
struct PatchCard sTestChaosPatch3 = {1, 10, "Effect 3"};
struct PatchCard sTestChaosPatch4 = {4, 3, "Effect 4"};

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
    0, FALSE,
};

void handle_patch_selection_inputs() {
    s32 previousSelection = gPatchSelectionMenu.selectedPatch;
    if(gPlayer1Controller->buttonPressed & D_JPAD) {
        gPatchSelectionMenu.selectedPatch += 2;
        if(gPatchSelectionMenu.selectedPatch > MAX_CARDS - 1) {
            gPatchSelectionMenu.selectedPatch = previousSelection;
        }
    } else if (gPlayer1Controller->buttonPressed & U_JPAD) {
        gPatchSelectionMenu.selectedPatch -= 2;
        if(gPatchSelectionMenu.selectedPatch < 0) {
            gPatchSelectionMenu.selectedPatch = previousSelection;
        }
    } else if(gPlayer1Controller->buttonPressed & R_JPAD) {
        gPatchSelectionMenu.selectedPatch++;
        if(gPatchSelectionMenu.selectedPatch > MAX_CARDS - 1) {
            gPatchSelectionMenu.selectedPatch = previousSelection;
        }
    } else if(gPlayer1Controller->buttonPressed & L_JPAD) {
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
    guScale(cardScaleMtx, scale, scale, 1.0f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(cardTransMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(cardScaleMtx),
          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    if(reverse) {
        gSPDisplayList(gDisplayListHead++, patch_bg_r_mesh_r_mesh);
    } else {
        gSPDisplayList(gDisplayListHead++, patch_bg_mesh_mesh);
    }
    
    //Draw patch quality beads
    gSPDisplayList(gDisplayListHead++, patch_quality_bead_begin);
    guTranslate(qualityTransMtx, 0, -40, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(qualityTransMtx++),
          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    for(int i = 0; i < quality; i++) {
        guTranslate(qualityTransMtx, 50, 0, 0);
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
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(cardScaleMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        guTranslate(timerTransMtx, 400, -40, 0);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(timerTransMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gSPDisplayList(gDisplayListHead++, star_timer_Mesh_mesh);
        //temp
        print_text_fmt_int(x + 110, y - 20, "%d" ,card->duration);
    }

    //temp
    print_text(x + 5, y - 35, card->patchName);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void display_patch_selection_ui() {
    f32 scale = 0.26f;
    s32 selectedPatch = gPatchSelectionMenu.selectedPatch;

    patch_bg_scroll();
    create_dl_ortho_matrix();
    
    //Ugly and hopefully temporary
    render_patch_card(10, SCREEN_HEIGHT - 10, scale * (1.0f + (0.05f * (selectedPatch == 0))), sAvailablePatches[0], FALSE);
    render_patch_card((SCREEN_WIDTH / 2) + 10, SCREEN_HEIGHT - 10, scale * (1.0f + (0.05f * (selectedPatch == 1))), sAvailablePatches[1], TRUE);
    render_patch_card(10, (SCREEN_HEIGHT / 2) + 30, scale * (1.0f + (0.05f * (selectedPatch == 2))), sAvailablePatches[2], FALSE);
    render_patch_card((SCREEN_WIDTH / 2) + 10, (SCREEN_HEIGHT / 2) + 30, scale * (1.0f + (0.05f * (selectedPatch == 3))), sAvailablePatches[3], TRUE);
}