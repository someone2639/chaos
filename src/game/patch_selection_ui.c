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
struct PatchCard sTestChaosPatch1 = {2, "Effect 1"};
struct PatchCard sTestChaosPatch2 = {3,  "Effect 2"};
struct PatchCard sTestChaosPatch3 = {1,  "Effect 3"};
struct PatchCard sTestChaosPatch4 = {4,  "Effect 4"};

u8 sQualityColors[][4] = {
    {0x9A, 0x9A, 0x9A},
    {0x20, 0xDB, 0x1D},
    {0x47, 0x42, 0xDB},
    {0x8B, 0x00, 0xC5},
};

struct PatchCard *sAvailablePatches[] = {
    &sTestChaosPatch1,
    &sTestChaosPatch2,
    &sTestChaosPatch3,
    &sTestChaosPatch4,
};

struct PatchSelectionMenu gPatchSelectionMenu = {
    0, 4, FALSE,
};

void handle_patch_selection_inputs() {
    if(gPlayer1Controller->buttonPressed & D_JPAD) {
        gPatchSelectionMenu.selectedPatch++;
        if(gPatchSelectionMenu.selectedPatch > gPatchSelectionMenu.numCards - 1) {
            gPatchSelectionMenu.selectedPatch = 0;
        }
    } else if (gPlayer1Controller->buttonPressed & U_JPAD) {
        gPatchSelectionMenu.selectedPatch--;
        if(gPatchSelectionMenu.selectedPatch < 0) {
            gPatchSelectionMenu.selectedPatch = gPatchSelectionMenu.numCards - 1;
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
    Draws a patch card at the given x, y coordinates
*/
void render_patch_card(f32 x, f32 y, f32 scale, struct PatchCard *card) {
    s32 quality = card->quality;
    s32 colorID = quality - 1;
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    Mtx *transMtx = alloc_display_list(sizeof(Mtx) * (quality + 2));

    gDPSetPrimColor(gDisplayListHead++, 0, 0, 
                    sQualityColors[colorID][0], sQualityColors[colorID][1], sQualityColors[colorID][2], 255);

    //Draw patch bg
    guScale(scaleMtx, scale, scale, 1.0f);
    guTranslate(transMtx, x, y, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx++),
          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gSPDisplayList(gDisplayListHead++, patch_bg_mesh_mesh);
    gSPDisplayList(gDisplayListHead++, patch_quality_bead_begin);
    
    //Draw patch quality beads
    guTranslate(transMtx, -240, 60, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    for(int i = 0; i < quality; i++) {
        guTranslate(transMtx, 50, 0, 0);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gSPDisplayList(gDisplayListHead++, patch_quality_bead);
    }
    gSPDisplayList(gDisplayListHead++, patch_quality_bead_end);

    //temp
    print_text_centered(x, y - 10, card->patchName);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void display_patch_selection_ui() {
    f32 scale = 0.26f;
    f32 cardScale;
    f32 startPoint = SCREEN_HEIGHT - 10;
    f32 endPoint = 10;
    f32 cardGap = (endPoint - startPoint) / gPatchSelectionMenu.numCards;
    f32 cardY; 
    f32 cardX = SCREEN_WIDTH - ((DEFAULT_CARD_WIDTH / 2) * scale) - 10;

    patch_bg_scroll();
    create_dl_ortho_matrix();
    
    for(int i = 0; i < gPatchSelectionMenu.numCards; i++) {
        if(i == gPatchSelectionMenu.selectedPatch) {
            cardScale = scale * 1.1f;
        } else {
            cardScale = scale;
        }
        cardY = startPoint + (cardGap * i) + (cardGap * 0.5f);
        render_patch_card(cardX, cardY, cardScale, sAvailablePatches[i]);
    }
}