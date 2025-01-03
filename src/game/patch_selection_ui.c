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
s32 numCards = 2;

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

void display_patch_selection_ui() {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx) * numCards);
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx) * numCards);

    f32 cardScale = 0.27f;
    s32 cardHeight = (DEFAULT_CARD_HEIGHT * cardScale);
    f32 startPoint = 10;
    f32 endPoint = SCREEN_HEIGHT  - startPoint;
    f32 cardGap = (endPoint - startPoint) / numCards;
    f32 cardY;
    f32 cardX = (SCREEN_WIDTH / 6) * 5;

    patch_bg_scroll();
    create_dl_ortho_matrix();
    gDPSetPrimColor(gDisplayListHead++, 0, 0, 255, 0, 255, 255);
    
    for(int i = 0; i < numCards; i++) {
        cardY = startPoint + (cardGap * i) + (cardGap * 0.5f);
        guScale(scaleMtx, cardScale, cardScale, 1.0f);
        guTranslate(transMtx, cardX, cardY, 0);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx++),
                  G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

        gSPDisplayList(gDisplayListHead++, patch_bg_mesh_mesh);

        gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
        print_text(cardX, cardY, "a");
    }
    //print_text(20, 20, sTestChaosPatch1.patchName);
}