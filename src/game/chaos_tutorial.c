#include "sm64.h"

#include "chaos_tutorial.h"
#include "audio/external.h"
#include "debug.h"
#include "fasttext.h"
#include "game_init.h"
#include "geo_misc.h"
#include "ingame_menu.h"
#include "memory.h"
#include "object_helpers.h"
#include "patch_selection_ui.h"
#include "segment2.h"
#include "chaos_menus.h"

#define G_CC_FILL         0,      0,      0,         PRIMITIVE,   0,      0,      0,         1
#define G_CC_FILLA        0,      0,      0,         PRIMITIVE,   0,      0,      0,         PRIMITIVE
#define G_CC_TEX          TEXEL0, 0,      PRIMITIVE, 0,           0,      0,      0,         TEXEL0
#define G_CC_TEXA         TEXEL0, 0,      PRIMITIVE, 0,           TEXEL0, 0,      PRIMITIVE, 0

#define STICK_DOWN (1 << 0)
#define STICK_UP (1 << 1)
#define STICK_LEFT (1 << 2)
#define STICK_RIGHT (1 << 3)
#define STICK_VERTICAL_MASK (STICK_UP | STICK_DOWN)
#define STICK_HORIZONTAL_MASK (STICK_LEFT | STICK_RIGHT)
#define STICK_DIRECTIONS 4
#define STICK_TRIGGER_ON 54.0f
#define STICK_TRIGGER_OFF 52.0f
#define STICK_INFLUENCE_MAX 0.08f

#define FRAMES_TO_CONTINUE 4
#define FRAMES_TO_WAIT_AFTER_FIRST 10

#define TUTORIAL_ANIM_FRAMES    9
#define TUTORIAL_DESC_START_Y   (-50)
#define TUTORIAL_DESC_END_Y     (SCREEN_CENTER_Y + 5)

static u8 inputStickFlags = 0;
static s8 stickHistoryFrames = 0;
static s8 stickShouldOverrideHold = TRUE;

static s32 animScaleRight = 0;
static s32 animScaleLeft = 0;
static s8 sTutorialAnimProg = 0;
static u8 sTutorialImgA = 0;
static f32 sTutorialDescY = TUTORIAL_DESC_START_Y;
static u8 sChsTutState;

static f32 animScaleLUT[] = {
    1.0f,
    0.993f,
    0.985f,
    0.97f,
    0.95f,
    0.925f,
    0.875f,
    0.80f,
};

static const Gfx dl_chstut_img_1cycle_begin[] = {
    gsDPPipeSync(),
    gsDPSetTexturePersp(G_TP_NONE),
    gsDPSetTextureFilter(G_TF_POINT),
    gsSPEndDisplayList(),
};

static const Gfx dl_chstut_img_1cycle_end[] = {
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsSPEndDisplayList(),
};

static void chstut_render_tiled_image(Gfx **dl, Texture *image, s32 x, s32 y, s32 width, s32 height, u8 r, u8 g, u8 b, u8 a) {
    Gfx *dlHead = *dl;

    if (a == 0) {
        return; // Don't even bother if it's invisible
    }

    s32 modeSC;
    s32 mOne;
    s32 maxWidth = 64;
    s32 maxHeight = 32;

    if (height > width) {
        maxWidth = 32;
        maxHeight = 64;
    }

    gSPDisplayList(dlHead++, dl_chstut_img_1cycle_begin);
    gDPSetPrimColor(dlHead++, 0, 0, r, g, b, a);

    if (a >= 0xFF) {
        gDPSetCombineMode(dlHead++, G_CC_TEX, G_CC_TEX);
        if (x >= 0 && (width % 4) == 0 && (r >= 0xFF && g >= 0xFF && b >= 0xFF)) {
            gDPSetRenderMode(dlHead++, G_RM_NOOP, G_RM_NOOP2);
            gDPSetCycleType(dlHead++, G_CYC_COPY);
            modeSC = 4;
            mOne = 1;
        } else {
            gDPSetRenderMode(dlHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
            gDPSetCycleType(dlHead++, G_CYC_1CYCLE);
            modeSC = 1;
            mOne = 0;
        }
    } else {
        gDPSetCombineMode(dlHead++, G_CC_TEXA, G_CC_TEXA);
        gDPSetRenderMode(dlHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetCycleType(dlHead++, G_CYC_1CYCLE);
        modeSC = 1;
        mOne = 0;
    }

    for (s32 curImgY = 0; curImgY < height; curImgY += maxHeight) {
        s32 tileHeight = MIN(height - curImgY, maxHeight);
        s32 heightMask = 0;
        for (s32 val = tileHeight - 1; val > 0; val >>= 1) {
            heightMask++;
        }

        for (s32 curImgX = 0; curImgX < width; curImgX += maxWidth) { 
            s32 tileWidth = MIN(width - curImgX, maxWidth);
            s32 widthMask = 0;
            for (s32 val = tileWidth - 1; val > 0; val >>= 1) {
                widthMask++;
            }

            gDPLoadSync(dlHead++);
            gDPLoadTextureTile(dlHead++,
                image, G_IM_FMT_RGBA, G_IM_SIZ_16b, width, height, curImgX, curImgY, ((curImgX + tileWidth) - 1), ((curImgY + tileHeight) - 1),
                0, (G_TX_NOMIRROR | G_TX_WRAP), (G_TX_NOMIRROR | G_TX_WRAP), widthMask, heightMask, G_TX_NOLOD, G_TX_NOLOD);
            gSPScisTextureRectangle(dlHead++,
                ((x + curImgX) << 2),
                ((y + curImgY) << 2),
                (((x + curImgX + tileWidth) - mOne) << 2),
                (((y + curImgY + tileHeight) - mOne) << 2),
                G_TX_RENDERTILE, 0, 0, (modeSC << 10), (1 << 10));
        }
    }

    gSPDisplayList(dlHead++, dl_chstut_img_1cycle_end);

    *dl = dlHead;
}

static void chstut_draw_shaded_background(Gfx** dl, s32 x1, s32 x2, s32 y1, s32 y2, u8 r, u8 g, u8 b, u8 a) {
    Gfx* dlHead = *dl;

    if (a == 0) {
        return; // Don't even bother if it's invisible
    }

    gDPPipeSync(dlHead++);
    gDPSetTextureFilter(dlHead++, G_TF_POINT);
    gDPSetCycleType(dlHead++, G_CYC_1CYCLE);

    if (a == 0xFF) {
        gDPSetRenderMode(dlHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
        gDPSetCombineMode(dlHead++, G_CC_FILL, G_CC_FILL);
    } else {
        gDPSetRenderMode(dlHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetCombineMode(dlHead++, G_CC_FILLA, G_CC_FILLA);
    }

    gDPSetPrimColor(dlHead++, 0, 0, r, g, b, a);
    gDPPipeSync(dlHead++);

    if (x1 > x2) {
        s32 tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2) {
        s32 tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    Vtx *verts = alloc_display_list(4 * sizeof(Vtx));
    if (verts != NULL) {
        make_vertex(verts, 0, x1, y1, 0, 0, 0, 255, 255, 255, 255);
        make_vertex(verts, 1, x2, y1, 0, 0, 0, 255, 255, 255, 255);
        make_vertex(verts, 2, x2, y2, 0, 0, 0, 255, 255, 255, 255);
        make_vertex(verts, 3, x1, y2, 0, 0, 0, 255, 255, 255, 255);

        gSPVertex(dlHead++, verts, 4, 0);
        gSP2Triangles(dlHead++, 0, 1, 2, 0x0, 0, 2, 3, 0x0);
    }

    gDPPipeSync(dlHead++);
    gDPSetPrimColor(dlHead++, 0, 0, 255, 255, 255, 255);
    gDPSetCombineMode(dlHead++, G_CC_SHADE, G_CC_SHADE);
    gDPSetRenderMode(dlHead++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gDPSetTextureFilter(dlHead++, G_TF_BILERP);
    gDPSetTexturePersp(dlHead++, G_TP_PERSP);

    *dl = dlHead;
}

static u8 chstut_attempt_selection_move(u8 movementMask) {
    u8 inputStickFlagsLast = inputStickFlags;

    // Check for change in stick inputs
    if (gPlayer1Controller->rawStickY <= -STICK_TRIGGER_ON || gPlayer1Controller->buttonDown & D_JPAD || gPlayer1Controller->buttonDown & D_CBUTTONS) {
        inputStickFlags |= STICK_DOWN;
    } else if (gPlayer1Controller->rawStickY > -STICK_TRIGGER_OFF) {
        inputStickFlags &= ~STICK_DOWN;
    }

    if (gPlayer1Controller->rawStickY >= STICK_TRIGGER_ON || gPlayer1Controller->buttonDown & U_JPAD || gPlayer1Controller->buttonDown & U_CBUTTONS) {
        inputStickFlags |= STICK_UP;
    } else if (gPlayer1Controller->rawStickY < STICK_TRIGGER_OFF) {
        inputStickFlags &= ~STICK_UP;
    }

    if (gPlayer1Controller->rawStickX <= -STICK_TRIGGER_ON || gPlayer1Controller->buttonDown & L_JPAD || gPlayer1Controller->buttonDown & L_CBUTTONS) {
        inputStickFlags |= STICK_LEFT;
    } else if (gPlayer1Controller->rawStickX > -STICK_TRIGGER_OFF) {
        inputStickFlags &= ~STICK_LEFT;
    }

    if (gPlayer1Controller->rawStickX >= STICK_TRIGGER_ON || gPlayer1Controller->buttonDown & R_JPAD || gPlayer1Controller->buttonDown & R_CBUTTONS) {
        inputStickFlags |= STICK_RIGHT;
    } else if (gPlayer1Controller->rawStickX < STICK_TRIGGER_OFF) {
        inputStickFlags &= ~STICK_RIGHT;
    }

    // If opposing directions are both active, cancel them out
    if ((inputStickFlags & STICK_VERTICAL_MASK) == STICK_VERTICAL_MASK) {
        inputStickFlags &= ~STICK_VERTICAL_MASK;
    }

    if ((inputStickFlags & STICK_HORIZONTAL_MASK) == STICK_HORIZONTAL_MASK) {
        inputStickFlags &= ~STICK_HORIZONTAL_MASK;
    }

    // Remove stick inputs unrepresentative of scroll mask
    inputStickFlags &= movementMask;

    u8 appliedInput = 0;

    u8 anyInputsHeld = FALSE;
    // Check whether to progress held inputs
    for (u32 i = 0; i < STICK_DIRECTIONS; i++) {
        if (!(inputStickFlags & (1 << i))) {
            continue;
        }
    
        anyInputsHeld = TRUE;
        if (stickHistoryFrames == 0) {
            appliedInput |= (1 << i);
        } else if (stickHistoryFrames >= FRAMES_TO_WAIT_AFTER_FIRST && ((stickHistoryFrames - FRAMES_TO_WAIT_AFTER_FIRST) % FRAMES_TO_CONTINUE) == 0) {
            appliedInput |= (1 << i);
            stickHistoryFrames = FRAMES_TO_WAIT_AFTER_FIRST;
            stickShouldOverrideHold = FALSE;
        } else if (stickShouldOverrideHold && !(inputStickFlagsLast & (1 << i))) {
            appliedInput |= (1 << i);
        }
    }

    if (!anyInputsHeld) {
        stickHistoryFrames = 0;
        stickShouldOverrideHold = TRUE;
    } else {
        stickHistoryFrames++;
    }    

    return appliedInput;
}

static void chstut_load_image(u8 *imgAddress) {
    if (imgAddress == gChaosTutorialLoadedAddr) {
        return;
    }
    gChaosTutorialLoadedAddr = imgAddress;

    if (imgAddress == NULL) {
        bzero(gChaosTutorialImgBuffer, sizeof(gChaosTutorialImgBuffer));
        return;
    }

    dma_read(gChaosTutorialImgBuffer, (u8 *) imgAddress, (u8 *) imgAddress + (size_t) CHAOS_TUTORIAL_IMG_SIZE);
}

void chstut_bg_scroll(void) {
	int i = 0;
	int count = 4;
	int width = 64 * 0x20;
	int height = 32 * 0x20;

	static int currentX = 0;
	int deltaX;
	static int currentY = 0;
	int deltaY;
	Vtx *vertices = segmented_to_virtual(chstut_bg_mesh_mesh_vtx_0);

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

static void chstut_render_tutorial_description(Gfx** dl) {
    Gfx* dlHead = *dl;
    f32 scale = 1.0f;
    enum FastTextFont font = FT_FONT_SMALL_THIN;

    create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, SCREEN_CENTER_X, sTutorialDescY, 0.0f);
    create_dl_scale_matrix(&dlHead, MENU_MTX_NOPUSH, 1.0f, scale, 1.0f);
    gSPDisplayList(dlHead++, chstut_bg_mesh_mesh);

    if (*dl == gDisplayListHead) {
        *dl = dlHead;
    }

    if (gChaosTutorialSlides[gChaosTutorialSlideIndex].description != NULL) {
        slowtext_setup_ortho_rendering(font);
        slowtext_draw_ortho_text_linebreaks(-142, -77, DESC_STRING_WIDTH, gChaosTutorialSlides[gChaosTutorialSlideIndex].description,
            FT_FLAG_ALIGN_LEFT, 0xDF, 0xDF, 0xDF, 0xFF);
        slowtext_finished_rendering();
    }

    if (*dl == gDisplayListHead) {
        dlHead = *dl;
    }

    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    *dl = dlHead;
}

static void chstut_render_scroll_arrows(Gfx** dl) {
    Gfx* dlHead = *dl;

    const f32 scale = 2.0f;
    const f32 leftScale = animScaleLUT[animScaleLeft] * scale;
    const f32 rightScale = animScaleLUT[animScaleRight] * scale;
    const s32 x = (SCREEN_CENTER_X * 4) / 5;
    const s32 y = (SCREEN_HEIGHT - 12) - ((CHAOS_TUTORIAL_IMG_HEIGHT + 1) / 2);

    create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, SCREEN_CENTER_X + x, y, 0.0f);
    create_dl_scale_matrix(&dlHead, MENU_MTX_NOPUSH, rightScale, rightScale, rightScale);
    gDPPipeSync(dlHead++);
    if (gChaosTutorialSlideIndex < (gChaosTutorialSlideCount - 1)) {
        gDPSetEnvColor(dlHead++, 255, 255, 255, sTutorialImgA);
    } else {
        gDPSetEnvColor(dlHead++, 255, 255, 255, ((f32)sTutorialImgA / 0xFF) * 31);
    }
    gSPDisplayList(dlHead++, dl_draw_triangle_centered);
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
    
    create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, SCREEN_CENTER_X - x, y, 0.0f);
    create_dl_scale_matrix(&dlHead, MENU_MTX_NOPUSH, -leftScale, -leftScale, leftScale);
    gDPPipeSync(dlHead++);
    if (gChaosTutorialSlideIndex > 0) {
        gDPSetEnvColor(dlHead++, 255, 255, 255, sTutorialImgA);
    } else {
        gDPSetEnvColor(dlHead++, 255, 255, 255, ((f32)sTutorialImgA / 0xFF) * 31);
    }
    gSPDisplayList(dlHead++, dl_draw_triangle_centered);
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    if (animScaleLeft > 0) {
        animScaleLeft--;
    }
    if (animScaleRight > 0) {
        animScaleRight--;
    }

    *dl = dlHead;
}

void chstut_tutorial_init(void) {
    gChaosTutorialSlideIndex = 0;
    inputStickFlags = 0;
    stickHistoryFrames = 0;
    stickShouldOverrideHold = TRUE;
    animScaleRight = 0;
    animScaleLeft = 0;
    sTutorialAnimProg = 0;
    sTutorialImgA = 0;
    sTutorialDescY = TUTORIAL_DESC_START_Y;
    sChsTutState = CHS_TUT_STATE_STARTING;
}

s32 chstut_move_right(void) {
    if(gChaosTutorialSlideIndex < (gChaosTutorialSlideCount - 1)) {
        gChaosTutorialSlideIndex++;
        animScaleRight = ARRAY_COUNT(animScaleLUT) - 1;
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
        return TRUE;
    } else {
        return FALSE;
    }
}

s32 chstut_move_left(void) {
    if(gChaosTutorialSlideIndex > 0) {
        gChaosTutorialSlideIndex--;
        animScaleLeft = ARRAY_COUNT(animScaleLUT) - 1;
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
        return TRUE;
    } else {
        return FALSE;
    }
}

s32 chstut_update_tutorial(void) {
    s32 end = FALSE;
    u8 stickMove = chstut_attempt_selection_move(STICK_HORIZONTAL_MASK);

    sTutorialImgA = (0xFF * ((f32)sTutorialAnimProg / TUTORIAL_ANIM_FRAMES));
    sTutorialDescY = (TUTORIAL_DESC_START_Y + (TUTORIAL_DESC_END_Y - TUTORIAL_DESC_START_Y) * sins((0x3FFF / TUTORIAL_ANIM_FRAMES) * sTutorialAnimProg));

    switch(sChsTutState) {
        case CHS_TUT_STATE_STARTING:
            if(sTutorialAnimProg == 0) {
                play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
            }
            if(sTutorialAnimProg < TUTORIAL_ANIM_FRAMES) {
                sTutorialAnimProg++;
            } else {
                sChsTutState = CHS_TUT_STATE_DEFAULT;
            }
            break;
        case CHS_TUT_STATE_ENDING:
            if(sTutorialAnimProg == TUTORIAL_ANIM_FRAMES) {
                play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);
            }
            if(sTutorialAnimProg > 0) {
                sTutorialAnimProg--;
            } else {
                end = TRUE;
            }
            break;
        case CHS_TUT_STATE_DEFAULT:
        default:
            if(gPlayer1Controller->buttonPressed & (START_BUTTON | Z_TRIG | L_TRIG)) {
                sChsTutState = CHS_TUT_STATE_ENDING;
            } else if (gPlayer1Controller->buttonPressed & A_BUTTON) {
                if(!chstut_move_right()) {
                    sChsTutState = CHS_TUT_STATE_ENDING;
                }
            } else if (gPlayer1Controller->buttonPressed & B_BUTTON) {
                if(!chstut_move_left()) {
                    sChsTutState = CHS_TUT_STATE_ENDING;
                }
            } else if (stickMove & STICK_RIGHT) {
                if(!chstut_move_right()) {
                   play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
                }
            } else if (stickMove & STICK_LEFT) {
                if(!chstut_move_left()) {
                    play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
                }
            }
    }
    
    return end;
}

void chstut_render_tutorial(void) {
    chstut_load_image(gChaosTutorialSlides[gChaosTutorialSlideIndex].imageAddress);

    create_dl_ortho_matrix(&gDisplayListHead);

    chstut_draw_shaded_background(&gDisplayListHead, 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 0, 0, ((f32)sTutorialImgA / 0xFF) * 200);
    if (gChaosTutorialLoadedAddr != NULL) {
        chstut_render_tiled_image(&gDisplayListHead, gChaosTutorialImgBuffer, SCREEN_CENTER_X - ((CHAOS_TUTORIAL_IMG_WIDTH + 1) / 2), 9,
            CHAOS_TUTORIAL_IMG_WIDTH, CHAOS_TUTORIAL_IMG_HEIGHT, 255, 255, 255, sTutorialImgA);
    }
    chstut_render_scroll_arrows(&gDisplayListHead);

    chstut_bg_scroll();
    chstut_render_tutorial_description(&gDisplayListHead);

    if(sChsTutState == CHS_TUT_STATE_DEFAULT) {
        menu_start_button_prompt();
        menu_button_prompt(SCREEN_WIDTH - 32, SCREEN_HEIGHT - 23, MENU_PROMPT_A_BUTTON);
        menu_button_prompt(SCREEN_WIDTH - 70, SCREEN_HEIGHT - 23, MENU_PROMPT_B_BUTTON);
        menu_button_prompt(SCREEN_WIDTH - 112, SCREEN_HEIGHT - 23, MENU_PROMPT_START_BUTTON);
        menu_end_button_prompt();
        fasttext_setup_textrect_rendering(FT_FONT_SMALL_THIN);
        fasttext_draw_texrect(SCREEN_WIDTH - 33, SCREEN_HEIGHT - 23, "Next", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
        fasttext_draw_texrect(SCREEN_WIDTH - 72, SCREEN_HEIGHT - 23, "Back", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
        fasttext_draw_texrect(SCREEN_WIDTH - 114, SCREEN_HEIGHT - 23, "Return", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
        fasttext_finished_rendering();
    }
}
