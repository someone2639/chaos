#include <ultra64.h>
#include <PR/gbi.h>

#include "chaos_menus.h"
#include "sm64.h"
#include "game_init.h"
#include "segment2.h"
#include "fasttext.h"
#include "debug.h"
#include "geo_misc.h"
#include "engine/math_util.h"
#include "object_helpers.h"

struct ButtonTexturePair sButtonTextureTable[MENU_PROMPT_COUNT] = {
    [MENU_PROMPT_A_BUTTON]      = {.up = texture_icon_a_button,     .down = texture_icon_a_button_down},
    [MENU_PROMPT_B_BUTTON]      = {.up = texture_icon_b_button,     .down = texture_icon_b_button_down},
    [MENU_PROMPT_START_BUTTON]  = {.up = texture_icon_start_button, .down = texture_icon_start_button_down},
    [MENU_PROMPT_L_TRIG]        = {.up = texture_icon_l_trig,       .down = texture_icon_l_trig_down},
    [MENU_PROMPT_R_TRIG]        = {.up = texture_icon_r_trig,       .down = texture_icon_r_trig_down},
    [MENU_PROMPT_Z_TRIG]        = {.up = texture_icon_z_trig,       .down = texture_icon_z_trig_down},
    [MENU_PROMPT_C_DOWN]        = {.up = texture_icon_c_d_button,   .down = texture_icon_c_d_button_down},
    [MENU_PROMPT_C_LEFT]        = {.up = texture_icon_c_l_button,   .down = texture_icon_c_l_button_down},
    [MENU_PROMPT_C_UP]          = {.up = texture_icon_c_u_button,   .down = texture_icon_c_u_button_down},
    [MENU_PROMPT_C_RIGHT]       = {.up = texture_icon_c_r_button,   .down = texture_icon_c_r_button_down},
};

/*
    Sets the menu anim id and resets timer and phase
*/
void menu_play_anim(struct ChaosMenu *menu, s32 animId){
    menu->animTimer = 0;
    menu->animPhase = 0;
    menu->animId = animId;
}

/*
    Updates the menu state
*/
void menu_set_state(struct ChaosMenu *menu, u32 state) {
    menu->menuState = state;
}

struct {
    u8 holdTimer;
    u8 lastDir : 4;
    u8 currentDir : 4;
} sMenuInputHandler;

/*
    Updates the last held directional input for menu navigation. Prioritizes joystick input over dpad input.
    Skips joystick input when held on consecutive frames, unless joystick is held for a certain amount of time,
    at which point it will skip every few frames instead.
*/
void menu_update_input_dir() {
    f32 stickX = gPlayer1Controller->rawStickX;
    f32 stickY = gPlayer1Controller->rawStickY;
    u32 pressed = gPlayer1Controller->buttonPressed;
    u32 dir = MENU_DIR_NONE;

    // Check stick inputs first

    // Check up/down
    if(stickY > 60) {
        dir |= MENU_DIR_U;
    } else if(stickY < -60) {
        dir |= MENU_DIR_D;
    }

    // Check left/right
    if (stickX > 60) {
        dir |= MENU_DIR_R;
    } else if (stickX < -60) {
        dir |= MENU_DIR_L;
    }

    if (dir != MENU_DIR_NONE) {
        // Determine if stick input should be read again, otherwise increase hold timer
        if (dir != sMenuInputHandler.lastDir) {
            sMenuInputHandler.lastDir = dir;
            sMenuInputHandler.holdTimer = 0;
        } else if (sMenuInputHandler.holdTimer >= MENU_INPUT_IGNORE_FRAMES) {
            sMenuInputHandler.lastDir = dir;
            sMenuInputHandler.holdTimer -= MENU_INPUT_HOLD_SKIP_FRAMES;
        } else {
            dir = MENU_DIR_NONE;
            sMenuInputHandler.holdTimer++;
        }
    } else {
        sMenuInputHandler.holdTimer = 0;
        sMenuInputHandler.lastDir = MENU_DIR_NONE;

        // If no joystick input was read, check dpad inputs

        // Check up/down
        if(pressed & U_JPAD) {
            dir |= MENU_DIR_U;
        } else if (pressed & D_JPAD) {
            dir |= MENU_DIR_D;
        }

        // Check left/right
        if (pressed & R_JPAD) {
            dir |= MENU_DIR_R;
        } else if (pressed & L_JPAD) {
            dir |= MENU_DIR_L;
        }
    }

    sMenuInputHandler.currentDir = dir;
}

u32 menu_get_input_dir() {
    return sMenuInputHandler.currentDir;
}

/*
    Navigates a horizontal menu scheme from min to max. Returns TRUE if index changed.
*/
s32 menu_navigate_horizontal(s32 *curIndex, s32 min, s32 max, s32 wrap) {
    u32 dir = menu_get_input_dir();
    s32 newIndex = *curIndex;
    s32 oldIndex = *curIndex;

    if(dir & MENU_DIR_R) {
        if(++newIndex > (max - 1)) {
            if(wrap) {
                newIndex = min;
            } else {
                newIndex = (max - 1);
            }
        }
    } else if (dir & MENU_DIR_L) {
        if(--newIndex < min) {
            if(wrap) {
                newIndex = (max - 1);
            } else {
                newIndex = min;
            }
        }
    }

    *curIndex = newIndex;
    return (newIndex != oldIndex);
}

/*
    Navigates a vertical menu scheme from min to max. Returns TRUE if index changed.
*/
s32 menu_navigate_vertical(s32 *curIndex, s32 min, s32 max, s32 wrap) {
    u32 dir = menu_get_input_dir();
    s32 newIndex = *curIndex;
    s32 oldIndex = *curIndex;

    if(dir & MENU_DIR_D) {
        if(++newIndex > (max - 1)) {
            if(wrap) {
                newIndex = min;
            } else {
                newIndex = (max - 1);
            }
        }
    } else if (dir & MENU_DIR_U) {
        if(--newIndex < min) {
            if(wrap) {
                newIndex = (max - 1);
            } else {
                newIndex = min;
            }
        }
    }

    *curIndex = newIndex;
    return (newIndex != oldIndex);
}

/*
    Updates the animations for a menu. Returns true if the animation has finished.
*/
s32 menu_update_anims(struct ChaosMenu *menu, s32 (*animFunctions[])(void)) {
    s32 animId = menu->animId;
    s32 animFrames = menu->animFrames;

    s32 (*animFunc)(void) = animFunctions[animId];
    s32 isFinished = animFunc();

    if(isFinished) {
        return TRUE;
    } else {
        if(++menu->animTimer > animFrames) {
            menu->animTimer = 0;
            menu->animPhase++;
        }
        return FALSE;
    }
}

ALWAYS_INLINE f32 menu_calc_anim_percent(f32 prog, s32 easeType) {
    if(prog > 1.0f) return 1.0f;
    if(prog < 0.0f) return 0.0f;
    
    switch(easeType) {
        case MENU_EASE_NONE:
        default:
            return (prog);
            break;
        case MENU_EASE_IN:
            return (1.0f - coss((0x4000) * prog));
            break;
        case MENU_EASE_OUT:
            return (sins((0x4000) * prog));
            break;
        case MENU_EASE_BOTH:
            return ((coss((0x8000) * prog) / -2.0f) + 0.5f);
            break;
    }
}

s32 menu_anim_s32(f32 prog, s32 easeType, s32 start, s32 end) {
    s32 value;

    f32 animPercent = menu_calc_anim_percent(prog, easeType);
    s32 diff = end - start;

    value = start + (diff * animPercent);

    return value;
}

f32 menu_anim_f32(f32 prog, s32 easeType, f32 start, f32 end) {
    f32 value;

    f32 animPercent = menu_calc_anim_percent(prog, easeType);
    f32 diff = end - start;

    value = start + (diff * animPercent);

    return value;
}

/*
    Sets up to draw a button texture
*/
void menu_start_button() {
    gDPPipeSync(gDisplayListHead++);
	gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
	gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
	gDPSetAlphaCompare(gDisplayListHead++, G_AC_THRESHOLD);
	gDPSetBlendColor(gDisplayListHead++, 255, 255, 255, 255);
    gDPPipeSync(gDisplayListHead++);
    gSPTexture(gDisplayListHead++, 65535, 65535, 0, 0, 1);
}

/*
    Resets everything after drawing a button texture
*/
void menu_end_button() {
    gDPPipeSync(gDisplayListHead++);
	gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
	gSPTexture(gDisplayListHead++, 65535, 65535, 0, G_TX_RENDERTILE, G_OFF);
	gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
	gDPSetAlphaCompare(gDisplayListHead++, G_AC_NONE);
}

/*
    Draws a button texture
*/
void menu_draw_button(s32 x, s32 y, s32 button, s32 pressed) {
    u8 *buttonTexture = (pressed) ? sButtonTextureTable[button].down : sButtonTextureTable[button].up;
    buttonTexture = segmented_to_virtual(buttonTexture);

    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, buttonTexture);
	gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 0, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0);
	gDPLoadBlock(gDisplayListHead++, 7, 0, 0, 255, 512);
	gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 4, 0, G_TX_WRAP | G_TX_NOMIRROR, 4, 0);
	gDPSetTileSize(gDisplayListHead++, 0, 0, 0, 60, 60);
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + 15) << 2, (y + 15) << 2,
                        G_TX_RENDERTILE, 0, 0, 4 << 10, 1 << 10);
    gDPPipeSync(gDisplayListHead++);
}

/*
    Adds a button prompt to the button prompt list
*/
void menu_add_button_prompt(struct ButtonPromptList *list, enum MenuButtonPrompt button, char *text) {
    struct ButtonPrompt *prompt = alloc_display_list(sizeof(struct ButtonPrompt));
    prompt->button = button;
    prompt->text = text;
    prompt->next = NULL;

    // Get width of text
    int xPos = 0;
    int i = 0;

    if (!text) {
        assert(FALSE, "Attempted to pass NULL string to button prompt func!");
    }

    const struct FastTextProps *fontProps = &gFasttextFonts[FT_FONT_SMALL_THIN];
    const unsigned char *kerningTable = fontProps->kerningTable;

    while (text[i] != '\0' && text[i] != '\n') {
        int curChar = text[i];
        int charIndex = curChar - ' ';
        int tabCount;

        switch (curChar) {
            case '\t': // tab
                tabCount = (xPos + FT_TAB_WIDTH) / FT_TAB_WIDTH;
                xPos = tabCount * FT_TAB_WIDTH;
                break;
            case '@': // string color
                i += 8;
                break;
            default:
                xPos += kerningTable[charIndex];
                break;
        }

        i++;
    }

    xPos -= fontProps->subFromLength;
    if (xPos < 0) {
        xPos = 0;
    }

    prompt->offset = xPos + 22;

    if(list->tail) {
        list->tail->next = prompt;
    }
    if(!list->head) {
        list->head = prompt;
    }
    list->tail = prompt;
}

/*
    Renders the contents of a button prompt list. First draws all the buttons, then all the text.
    Always draws right aligned button prompts, going horizontally to the left.
*/
void menu_render_button_prompt_list(s32 x, s32 y, struct ButtonPromptList *list) {
    s32 pressed = ((gGlobalTimer % 60) > 30);
    // Draw buttons
    menu_start_button();
    s32 offset = 0;
    for(struct ButtonPrompt *prompt = list->head; prompt != NULL; prompt = prompt->next) {
        menu_draw_button(x + offset, y, prompt->button, pressed);
        offset -= prompt->offset;
    }
    menu_end_button();

    // Draw text
    fasttext_setup_textrect_rendering(FT_FONT_SMALL_THIN);
    offset = -2;
    for(struct ButtonPrompt *prompt = list->head; prompt != NULL; prompt = prompt->next) {
        fasttext_draw_texrect(x + offset, y, prompt->text, FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
        offset -= prompt->offset;
    }
    fasttext_finished_rendering();
}

/*
    Draws a single button prompt
*/
void menu_single_button_prompt(s32 x, s32 y, enum MenuButtonPrompt button, char *text, s32 alignLeft) {
    s32 pressed = ((gGlobalTimer % 60) > 30);

    menu_start_button();
    menu_draw_button(x, y, button, pressed);
    menu_end_button();

    s32 offset, align;
    if(alignLeft) {
        offset = 18;
        align = FT_FLAG_ALIGN_LEFT;
    } else {
        offset = -2;
        align = FT_FLAG_ALIGN_RIGHT;
    }

    fasttext_setup_textrect_rendering(FT_FONT_SMALL_THIN);
    fasttext_draw_texrect(x + offset, y, text, align, 0xFF, 0xFF, 0xFF, 0xFF);
    fasttext_finished_rendering();
}

Gfx chaos_text_bg_start[] = {
	gsDPPipeSync(),
	gsSPLoadGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
	gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE),
	gsSPSetOtherMode(G_SETOTHERMODE_H, 4, 20, G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE | G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_1PRIMITIVE),
	gsSPSetOtherMode(G_SETOTHERMODE_L, 0, 32, G_AC_NONE | G_ZS_PIXEL | AA_EN | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL | GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) | GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b_LOAD_BLOCK, 1, desc_bg_desc_bg_i8),
	gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b_LOAD_BLOCK, 0, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadBlock(7, 0, 0, 1023, 256),
	gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b, 8, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0),
	gsDPSetTileSize(0, 0, 0, 252, 124),
    gsSPEndDisplayList(),
};

Gfx chaos_text_bg_outer[] = {
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
	gsDPPipeSync(),
	gsDPSetCombineLERP(SHADE, 0, PRIMITIVE, 0, 0, 0, 0, 1, SHADE, 0, PRIMITIVE, 0, 0, 0, 0, 1),
	gsSPSetOtherMode(G_SETOTHERMODE_L, 0, 3, G_AC_NONE | G_ZS_PIXEL),
	gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsSPEndDisplayList(),
};

Gfx chaos_text_bg_end[] = {
	gsDPPipeSync(),
	gsSPSetOtherMode(G_SETOTHERMODE_H, 4, 20, G_CD_MAGICSQ | G_AD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE | G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_1PRIMITIVE),
	gsSPEndDisplayList(),
};

void scroll_chaos_text_bg_uvs(Vec2s uvlr, Vec2s uvur, Vec2s uvul, Vec2s uvll) {
    int width = 128 * 0x20;
	int height = 64 * 0x20;
	int deltaX;
	int deltaY;

    deltaX = ((int)(0.05f * 0x20) % width) * gGlobalTimer;
	deltaY = ((int)(0.1f * 0x20) % height) * gGlobalTimer;

    if (absi(deltaX) > width) {
		deltaX -= (int)(absi(deltaX) / width) * width * signum_positive(deltaX);
	}
	if (absi(deltaY) > height) {
		deltaY -= (int)(absi(deltaY) / height) * height * signum_positive(deltaY);
	}

    uvlr[0] += deltaX;
    uvlr[1] += deltaY;

    uvur[0] += deltaX;
    uvur[1] += deltaY;

    uvul[0] += deltaX;
    uvul[1] += deltaY;

    uvll[0] += deltaX;
    uvll[1] += deltaY;

}

/*
    Calculates where the uvs of the chaos text background vertices should be
*/
void calc_chaos_text_bg_uv(Vec2s uv, s32 x, s32 y) {
    f32 uvx, uvy;
    f32 angSin = sins(((16) * 0x10000 / 360));
    f32 angCos = coss(((16) * 0x10000 / 360));
    
    uvx = (x * angCos) - (-y * angSin);
    uvy = (-y * angCos) + (x * angSin);

    uvx *= 0.8f;
    uvy *= 0.6f;

    uv[0] = ((s16)(uvx * 32.f));
    uv[1] = ((s16)(uvy * 32.f));
}

/*
    Create the background box used for most text display in the Rogue Chaos menus. Centered on 0, 0.
    The x/y coordinates are used to create a tiling effect when multiple backgrounds are
    used in conjunction. These should be set to the final resting position of the background, though this
    is only important if multiple backgrounds are used in the same menu scene.
*/
Gfx *menu_create_chaos_text_bg(s32 bgx, s32 bgy, s32 width, s32 height, u8 opacity) {
    Vtx *vtxBuf = alloc_display_list(sizeof(Vtx) * 8);
    s32 halfw = width / 2;
    s32 halfh = height / 2;

    Vec2s uvlr; calc_chaos_text_bg_uv(uvlr, bgx - halfw + 3, bgy + halfh - 2);
    Vec2s uvur; calc_chaos_text_bg_uv(uvur, bgx - halfw + 3, bgy - halfh + 2);
    Vec2s uvul; calc_chaos_text_bg_uv(uvul, bgx + halfw - 3, bgy - halfh + 2);
    Vec2s uvll; calc_chaos_text_bg_uv(uvll, bgx + halfw - 3, bgy + halfh - 2);

    scroll_chaos_text_bg_uvs(uvlr, uvur, uvul, uvll);
    
    // Inner
    make_vertex(vtxBuf, 0, -halfw + 3, halfh - 2, 0, uvlr[0], uvlr[1], 0xFF, 0xFF, 0xFF, 0xFF);
    make_vertex(vtxBuf, 1, -halfw + 3, -halfh + 2, 0, uvur[0], uvur[1], 0xFF, 0xFF, 0xFF, 0xFF);
    make_vertex(vtxBuf, 2, halfw - 3, -halfh + 2, 0, uvul[0], uvul[1], 0xFF, 0xFF, 0xFF, 0xFF);
    make_vertex(vtxBuf, 3, halfw - 3, halfh - 2, 0, uvll[0], uvll[1], 0xFF, 0xFF, 0xFF, 0xFF);

    // Outer
    make_vertex(vtxBuf, 4, -halfw, halfh, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    make_vertex(vtxBuf, 5, halfw, halfh, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    make_vertex(vtxBuf, 6, halfw, -halfh, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    make_vertex(vtxBuf, 7, -halfw, -halfh, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);

    Gfx *bg = alloc_display_list(sizeof(Gfx) * 11);
    Gfx *head = bg;

    // Draw border
	gDPSetPrimColor(head++, 0, 0, 25, 25, 25, opacity);
    gSPDisplayList(head++, chaos_text_bg_start);
    gSPVertex(head++, vtxBuf, 8, 0);
    gSP2Triangles(head++, 0, 1, 2, 0, 0, 2, 3, 0);

    // Draw inner rectangle
    gSPDisplayList(head++, chaos_text_bg_outer);
    gSP2Triangles(head++, 1, 0, 4, 0, 0, 5, 4, 0);
    gSP2Triangles(head++, 0, 3, 5, 0, 3, 6, 5, 0);
    gSP2Triangles(head++, 3, 2, 6, 0, 2, 7, 6, 0);
    gSP2Triangles(head++, 2, 1, 7, 0, 1, 4, 7, 0);
    gSPDisplayList(head++, chaos_text_bg_end);
    gSPEndDisplayList(head++);

    return bg;
}

/*
    Creates a cursor graphic, centered on the passed x, y, coordinates.
*/
Gfx *menu_create_cursor(s32 x, s32 y, f32 scale, u8 r, u8 g, u8 b, u8 a) {
    Mtx *transMtx = alloc_display_list(sizeof(Mtx));
    Mtx *scaleMtx = alloc_display_list(sizeof(Mtx));
    Gfx *cursor = alloc_display_list(sizeof(Gfx) * 6);
    Gfx *head = cursor;

    guTranslate(transMtx, x, y, 0);
    gSPMatrix(head++, VIRTUAL_TO_PHYSICAL(transMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

    guScale(scaleMtx, scale, scale, 1.0f);
    gSPMatrix(head++, VIRTUAL_TO_PHYSICAL(scaleMtx),
            G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

    gDPSetEnvColor(head++, r, g, b, a);
    gSPDisplayList(head++, dl_draw_triangle_centered);
    gSPPopMatrix(head++, G_MTX_MODELVIEW);
    gSPEndDisplayList(head++);

    return cursor;
}