#include <PR/ultratypes.h>

#include "sm64.h"
#include "geo_misc.h"

#include "area.h"
#include "engine/math_util.h"
#include "level_update.h"
#include "levels/castle_inside/header.h"
#include "levels/ending/header.h"
#include "levels/rr/header.h"
#include "mario.h"
#include "mario_actions_cutscene.h"
#include "memory.h"
#include "object_list_processor.h"
#include "rendering_graph_node.h"
#include "save_file.h"
#include "segment2.h"
#include "fasttext.h"
#include "audio/external.h"
#include "ingame_menu.h"
#include "game_init.h"
#include "object_helpers.h"

/**
 * @file geo_misc.c
 * This file contains miscellaneous geo_asm scripts.
 *
 * In particular, it builds:
 *   - the light that shows the player where to look for Tower of the Wing Cap,
 *   - the flying carpets seen in Rainbow Ride, and
 *   - the end screen displaying Peach's delicious cake.
 */

#define NUM_FLYING_CARPET_VERTICES 21
extern const s16 flying_carpet_static_vertex_data[NUM_FLYING_CARPET_VERTICES];

static s16 sCurAreaTimer = 1;
static s16 sPrevAreaTimer = 0;
static s16 sFlyingCarpetRippleTimer = 0;

s8 gFlyingCarpetState;

/**
 * Create a vertex with the given parameters and insert it into `vtx` at
 * position `n`.
 *
 * Texture coordinates are s10.5 fixed-point, which means you should left-shift the actual coordinates by 5.
 */
#ifndef GBI_FLOATS
void make_vertex(Vtx *vtx, s32 n, s16 x, s16 y, s16 z, s16 tx, s16 ty, u8 r, u8 g, u8 b, u8 a) {
#else
void make_vertex(Vtx *vtx, s32 n, f32 x, f32 y, f32 z, s16 tx, s16 ty, u8 r, u8 g, u8 b, u8 a) {
#endif
    vtx[n].v.ob[0] = x;
    vtx[n].v.ob[1] = y;
    vtx[n].v.ob[2] = z;

    vtx[n].v.flag = 0;

    vtx[n].v.tc[0] = tx;
    vtx[n].v.tc[1] = ty;

    vtx[n].v.cn[0] = r;
    vtx[n].v.cn[1] = g;
    vtx[n].v.cn[2] = b;
    vtx[n].v.cn[3] = a;
}

/**
 * Round `num` to the nearest `s16`.
 */
s16 round_float(f32 num) {
    // Note that double literals are used here, rather than float literals.
    if (num >= 0.0f) {
        return num + 0.5f;
    } else {
        return num - 0.5f;
    }
}

/**
 * Create a display list for the light in the castle lobby that shows the
 * player where to look to enter Tower of the Wing Cap.
 */
Gfx *geo_exec_inside_castle_light(s32 callContext, struct GraphNode *node, UNUSED f32 mtx[4][4]) {
    s32 flags;
    struct GraphNodeGenerated *generatedNode;
    Gfx *displayListHead = NULL;
    Gfx *displayList = NULL;

    if (callContext == GEO_CONTEXT_RENDER) {
        flags = save_file_get_flags();
        if (gHudDisplay.stars >= 10 && !(flags & SAVE_FLAG_HAVE_WING_CAP)) {
            displayList = alloc_display_list(2 * sizeof(*displayList));

            if (displayList == NULL) {
                return NULL;
            } else {
                displayListHead = displayList;
            }

            generatedNode = (struct GraphNodeGenerated *) node;
            generatedNode->fnNode.node.flags = (generatedNode->fnNode.node.flags & 0xFF) | 0x500;

            gSPDisplayList(displayListHead++, dl_castle_lobby_wing_cap_light);
            gSPEndDisplayList(displayListHead);
        }
    }

    return displayList;
}

/**
 * Update static timer variables that control the flying carpets' ripple effect.
 */
Gfx *geo_exec_flying_carpet_timer_update(s32 callContext, UNUSED struct GraphNode *node,
                                         UNUSED f32 mtx[4][4]) {
    if (callContext != GEO_CONTEXT_RENDER) {
        sFlyingCarpetRippleTimer = 0;
        sPrevAreaTimer = gAreaUpdateCounter - 1;
        sCurAreaTimer = gAreaUpdateCounter;
        gFlyingCarpetState = FLYING_CARPET_IDLE;
    } else {
        sPrevAreaTimer = sCurAreaTimer;
        sCurAreaTimer = gAreaUpdateCounter;
        if (sPrevAreaTimer != sCurAreaTimer) {
            sFlyingCarpetRippleTimer += 0x400;
        }
    }

    return NULL;
}

/**
 * Create a display list for a flying carpet with dynamic ripples.
 */
Gfx *geo_exec_flying_carpet_create(s32 callContext, struct GraphNode *node, UNUSED f32 mtx[4][4]) {
    s16 n, row, col, x, y, z, tx, ty;
    Vtx *verts;
    struct GraphNodeGenerated *generatedNode = (struct GraphNodeGenerated *) node;

    s16 *sp64 = segmented_to_virtual(&flying_carpet_static_vertex_data);
    Gfx *displayList = NULL;
    Gfx *displayListHead = NULL;
    struct Object *curGraphNodeObject;

    if (callContext == GEO_CONTEXT_RENDER) {
        verts = alloc_display_list(NUM_FLYING_CARPET_VERTICES * sizeof(*verts));
        displayList = alloc_display_list(7 * sizeof(*displayList));
        displayListHead = displayList;

        if (verts == NULL || displayList == NULL) {
            return NULL;
        }

        generatedNode->fnNode.node.flags = (generatedNode->fnNode.node.flags & 0xFF) | 0x100;

        for (n = 0; n <= 20; n++) {
            row = n / 3;
            col = n % 3;

            x = sp64[n * 4 + 0];
            y = round_float(sins(sFlyingCarpetRippleTimer + (row << 12) + (col << 14)) * 20.0);
            z = sp64[n * 4 + 1];
            tx = sp64[n * 4 + 2];
            ty = sp64[n * 4 + 3];

            make_vertex(verts, n, x, y, z, tx, ty, 0, 127, 0, 255);
        }

        gSPDisplayList(displayListHead++, dl_flying_carpet_begin);

        // The forward half.
        gSPVertex(displayListHead++, verts, 12, 0);
        gSPDisplayList(displayListHead++, dl_flying_carpet_model_half);

        // The back half.
        gSPVertex(displayListHead++, verts + 9, 12, 0);
        gSPDisplayList(displayListHead++, dl_flying_carpet_model_half);

        gSPDisplayList(displayListHead++, dl_flying_carpet_end);
        gSPEndDisplayList(displayListHead);

        curGraphNodeObject = (struct Object *) gCurGraphNodeObject;
        if (gMarioObject->platform == curGraphNodeObject) {
            gFlyingCarpetState = FLYING_CARPET_MOVING_WITH_MARIO;
        } else if (curGraphNodeObject->oForwardVel != 0.0) {
            gFlyingCarpetState = FLYING_CARPET_MOVING_WITHOUT_MARIO;
        } else {
            gFlyingCarpetState = FLYING_CARPET_IDLE;
        }
    }

    return displayList;
}

u16 sEndCakeTimer = 0;
u8 sEndCakePhase = 0;
#define END_CAKE_P1_FRAMES 200
#define END_CAKE_P2_FRAMES 20

/**
 * Create a display list for the end screen with Peach's delicious cake.
 */
Gfx *geo_exec_cake_end_screen(s32 callContext, struct GraphNode *node, UNUSED f32 mtx[4][4]) {
    struct GraphNodeGenerated *generatedNode = (struct GraphNodeGenerated *) node;
    Gfx *displayList = NULL;
    Gfx *displayListHead = NULL;

    if (callContext == GEO_CONTEXT_RENDER) {
        displayList = alloc_display_list(6 * sizeof(*displayList));
        displayListHead = displayList;

        generatedNode->fnNode.node.flags = (generatedNode->fnNode.node.flags & 0xFF) | 0x100;
#ifdef VERSION_EU
        gSPDisplayList(displayListHead++, dl_cake_end_screen);
#else
        gSPDisplayList(displayListHead++, dl_proj_mtx_fullscreen);
#endif
#ifdef VERSION_EU
#ifdef EU_CUSTOM_CAKE_FIX
    gSPDisplayList(displayListHead++, dl_cake_end_screen_eu_fix);
#else
    switch (eu_get_language()) {
            case LANGUAGE_ENGLISH:
                gSPDisplayList(displayListHead++, dl_cake_end_screen_eu_070296F8);
                break;
            case LANGUAGE_FRENCH:
                gSPDisplayList(displayListHead++, dl_cake_end_screen_eu_07029768);
                break;
            case LANGUAGE_GERMAN:
                gSPDisplayList(displayListHead++, dl_cake_end_screen_eu_070297D8);
                break;
        }
#endif
#else
        f32 scale;
        f32 transX;
        f32 transY;

        switch(sEndCakePhase) {
            case 0:
                scale = 1.0f;
                transX = 0;
                transY = 0;
                if(sEndCakeTimer >= END_CAKE_P1_FRAMES) {
                    sEndCakePhase++;
                    sEndCakeTimer = 0;
                } else {
                    sEndCakeTimer++;
                }
                break;
            case 1:
                scale = 1.0f - ((sEndCakeTimer / (f32)END_CAKE_P2_FRAMES) * 0.5f);
                transX = (SCREEN_WIDTH / 2) * (sEndCakeTimer / (f32)END_CAKE_P2_FRAMES);
                transY = (SCREEN_HEIGHT / 4) * (sEndCakeTimer / (f32)END_CAKE_P2_FRAMES);
                if(sEndCakeTimer >= END_CAKE_P2_FRAMES) {
                    sEndCakePhase++;
                    sEndCakeTimer = 0;
                } else {
                    sEndCakeTimer++;
                }
                break;
            case 2:
            default:
                scale = 0.5f;
                transX = (SCREEN_WIDTH / 2);
                transY = (SCREEN_HEIGHT / 4);
        }

        Mtx *transMtx = alloc_display_list(sizeof(Mtx));
        Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
        guTranslate(transMtx, transX, transY, 0);
        gSPMatrix(displayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
        guScale(scaleMtx, scale, scale, 1.0f);
        gSPMatrix(displayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
                G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gSPDisplayList(displayListHead++, dl_cake_end_screen);
        gSPPopMatrix(displayListHead++, G_MTX_MODELVIEW);
#endif
        gSPEndDisplayList(displayListHead);
    }

    return displayList;
}

void scroll_stats_bg() {
	int i = 0;
	int count = 4;
	int width = 64 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(stats_bg_stats_mesh_mesh_vtx_0);

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

Gfx *geo_chaos_cake_stats(s32 callContext, UNUSED struct GraphNode *node, UNUSED f32 mtx[4][4]) {
    if(callContext == GEO_CONTEXT_RENDER) {
        scroll_stats_bg();
        if(sEndCakePhase >= 2) {
            create_dl_ortho_matrix(&gDisplayListHead);

            if(sEndCakeTimer == 0) {
                play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
            }

            f32 scale;
            if(sEndCakeTimer < 7) {
                scale = ((f32)sEndCakeTimer / 7.0f);
            } else {
                scale = 1.0f;
            }

            Mtx *transMtx = alloc_display_list(sizeof(Mtx));
            Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));

            guTranslate(transMtx, (SCREEN_WIDTH / 4), SCREEN_CENTER_Y, 0);
            gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(transMtx),
                          G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
            guScale(scaleMtx, 1.0f, scale, 1.0f);
            gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(scaleMtx),
                    G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
            gSPDisplayList(gDisplayListHead++, stats_bg_stats_mesh_mesh);
            gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

            fasttext_setup_textrect_rendering(FT_FONT_VANILLA_SHADOW);
            if(sEndCakeTimer > 15) {
                fasttext_draw_texrect((SCREEN_WIDTH - 16), (SCREEN_HEIGHT - 24), VERSION_STRING, FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
                fasttext_setup_textrect_rendering(FT_FONT_MEDIUM);
                fasttext_draw_texrect(SCREEN_CENTER_X, 15, "FINAL SCORE", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
                
                //Difficulty
                fasttext_draw_texrect(16, 40, "Difficulty:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
                switch(save_file_get_difficulty(gCurrSaveFileNum - 1)) {
                    case CHAOS_DIFFICULTY_EASY:
                        fasttext_draw_texrect(150, 40, "Easy", FT_FLAG_ALIGN_RIGHT, 0x05, 0xDf, 0x15, 0xFF);
                        break;
                    case CHAOS_DIFFICULTY_NORMAL:
                    default:
                        fasttext_draw_texrect(150, 40, "Normal", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
                        break;
                    case CHAOS_DIFFICULTY_HARD:
                        fasttext_draw_texrect(150, 40, "Hard", FT_FLAG_ALIGN_RIGHT, 0xFF, 0x15, 0x25, 0xFF);
                        break;
                }
            }
            if(sEndCakeTimer > 30) {
                //Gamemode
                fasttext_draw_texrect(16, 60, "Gamemode:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
                if(save_file_get_challenge_mode(gCurrSaveFileNum - 1)) {
                    fasttext_draw_texrect(150, 60, "Challenge", FT_FLAG_ALIGN_RIGHT, 0xBB, 0xA1, 0x24, 0xFF);
                } else {
                    fasttext_draw_texrect(150, 60, "Classic", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
                }
            }
            if(sEndCakeTimer > 45) {
                //Star count
                char starCountText[8];
                s32 starCount = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);
                sprintf(starCountText, "%d", starCount);
                fasttext_draw_texrect(16, 80, "Total Stars:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
                fasttext_draw_texrect(150, 80, starCountText, FT_FLAG_ALIGN_RIGHT, 0xD0, 0xC4, 0x00, 0xFF);
            }
            if(sEndCakeTimer > 60) {
                //Blue star count
                char blueStarCountText[8];
                s32 blueStarCount = save_file_get_blue_stars();
                sprintf(blueStarCountText, "%d", blueStarCount);
                fasttext_draw_texrect(16, 100, "Blue Stars:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
                fasttext_draw_texrect(150, 100, blueStarCountText, FT_FLAG_ALIGN_RIGHT, 0x47, 0x8D, 0xCE, 0xFF);
            }
            if(sEndCakeTimer > 75) {
                //Deaths
                char deathsText[8];
                s32 deathCount = save_file_get_death_count();
                sprintf(deathsText, "%d", deathCount);
                fasttext_draw_texrect(16, 120, "Total Deaths:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
                fasttext_draw_texrect(150, 120, deathsText, FT_FLAG_ALIGN_RIGHT, 0xFF, 0x15, 0x25, 0xFF);
            }
            if(sEndCakeTimer > 90) {
                //Game loads
                char loadsText[8];
                s32 loadCount = save_file_get_game_loads();
                sprintf(loadsText, "%d", loadCount);
                fasttext_draw_texrect(16, 140, "Game Loads:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
                fasttext_draw_texrect(150, 140, loadsText, FT_FLAG_ALIGN_RIGHT, 0x9F, 0x9F, 0x9F, 0xFF);
            }
            if(sEndCakeTimer > 105) {
                //Total Patches
                char totalPatchesText[8];
                s32 totalPatchesCount = save_file_get_total_patches();
                sprintf(totalPatchesText, "%d", totalPatchesCount);
                fasttext_draw_texrect(16, 160, "Total Patches", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
                fasttext_draw_texrect(150, 160, totalPatchesText, FT_FLAG_ALIGN_RIGHT, 0x9F, 0x9F, 0x9F, 0xFF);
            }
            if(sEndCakeTimer > 120) {
                //Play time
                u32 playTime = save_file_get_play_time();
                u32 hours = playTime / (30 * 60 * 60);
                u32 mins = (playTime - (hours * (30 * 60 * 60))) / (60 * 30);
                u32 secs = (playTime - (hours * (30 * 60 * 60)) - (mins * (60 * 30))) / 30;
                char playTimeText[32];
                sprintf(playTimeText, "%dh %dm %ds", hours, mins, secs);
                fasttext_draw_texrect(16, 180, "Play Time:", FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
                fasttext_draw_texrect(150, 180, playTimeText, FT_FLAG_ALIGN_RIGHT, 0x9F, 0x9F, 0x9F, 0xFF);
            } else {
                sEndCakeTimer++;
            }

            if(!(sEndCakeTimer % 15)) {
                play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
            }

            fasttext_finished_rendering();
        }
    }
    return NULL;
}