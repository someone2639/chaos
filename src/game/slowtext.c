#include <ultra64.h>
#include "macros.h"
#include "segment2.h"
#include "debug.h"
#include "game_init.h"
#include "geo_misc.h"
#include "ingame_menu.h"
#include "memory.h"
#include "fasttext.h"

#define TEX_ASCII_START '!'

#define FASTTEXT_CHAR_COUNT (128 - ' ')

#define G_CC_TEXT PRIMITIVE, 0, TEXEL0, 0, 0, 0, 0, TEXEL0
#define G_CC_TEXTA TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0

static Vtx *slowtext_char_vtx[32];

static void alloc_verts_at_index(s32 width, s32 height) {
    assert((width - 1) < ARRAY_COUNT(slowtext_char_vtx), "slowtext verts too large!");
    Vtx *verts = slowtext_char_vtx[width - 1];
    if (verts != NULL) {
        return;
    }

    verts = alloc_display_list(4 * sizeof(Vtx));
    make_vertex(verts, 0,     0,      0, 0, ((    0 << 5)), ((height << 5)), 255, 255, 255, 255);
    make_vertex(verts, 1, width,      0, 0, ((width << 5)), ((height << 5)), 255, 255, 255, 255);
    make_vertex(verts, 2, width, height, 0, ((width << 5)), ((     0 << 5)), 255, 255, 255, 255);
    make_vertex(verts, 3,     0, height, 0, ((    0 << 5)), ((     0 << 5)), 255, 255, 255, 255);

    slowtext_char_vtx[width - 1] = verts;
}

static void slowtext_draw_char(Gfx **dl, const struct FastTextProps *fontProps, const s32 c) {
    const s32 textureHeight = fontProps->textureHeight;
    const s32 charIndex = (c - ' ');

    Gfx *dlHead = *dl;

    if (charIndex < 0 || charIndex >= FASTTEXT_CHAR_COUNT) {
#ifdef DEBUG_ASSERTIONS
        char errMsg[64];
        sprintf(errMsg, "Invalid slowtext character: 0x%02X", c);
        error(errMsg);
#endif
        return;
    }

    const s32 renderWidth = fontProps->offsetTable[charIndex].renderWidth;
    const s32 textureOffset = fontProps->offsetTable[charIndex].horizontalOffset;

    // NOTE: 1 being added to each of these to fix rendering issues with tri misalignment
    // or something (which only happens with gSPModifyVertex for some reason?)
    const s32 s1 = ((textureOffset << 5) + 1) << 16;
    const s32 s2 = (((textureOffset + renderWidth) << 5) + 1) << 16;
    const s32 t1 = (0 << 5) + 1;
    const s32 t2 = (textureHeight << 5) + 1;

    alloc_verts_at_index(renderWidth, textureHeight);

    gSPVertex(dlHead++, slowtext_char_vtx[renderWidth - 1], 4, 0);
    gSPModifyVertex(dlHead++, 0, G_MWO_POINT_ST, (s1 + t2));
    gSPModifyVertex(dlHead++, 1, G_MWO_POINT_ST, (s2 + t2));
    gSPModifyVertex(dlHead++, 2, G_MWO_POINT_ST, (s2 + t1));
    gSPModifyVertex(dlHead++, 3, G_MWO_POINT_ST, (s1 + t1));

    gSP2Triangles(dlHead++, 0, 1, 2, 0x0, 0, 2, 3, 0x0);

    create_dl_translation_matrix(&dlHead, MENU_MTX_NOPUSH, fontProps->kerningTable[charIndex], 0.0f, 0.0f);

    *dl = dlHead;
}

void slowtext_draw_ortho_text(s32 x, s32 y, const char* string, enum FastTextFlags flags, s32 r, s32 g, s32 b, s32 a) {
    s32 i = 0;
    s32 xShift = 0;
    s32 xPos = x;
    s32 yPos;
    Gfx *dlHead = gDisplayListHead;
    u8 rgbaBaseColors[4] = { r, g, b, a };
    u8 rgbaCurrentColors[4] = { r, g, b, a };

    if (!string) {
        assert(FALSE, "Attempted to pass NULL string to slowtext!");
        return;
    }

    aggress(fasttextCachedFontId != FT_FONT_NONE, "Neglected slowtext setup");

    const struct FastTextProps *fontProps = &gFasttextFonts[fasttextCachedFontId];
    const u8 *kerningTable = fontProps->kerningTable;
    y += fontProps->yShift;
    yPos = y;

    gDPSetPrimColor(dlHead++, 0, 0, rgbaBaseColors[0], rgbaBaseColors[1], rgbaBaseColors[2], rgbaBaseColors[3]);

    if (!(flags & FT_FLAG_SKIP_COMBINER)) {
        if (a == 255) {
            gDPSetCombineMode(dlHead++, G_CC_TEXT, G_CC_TEXT);
        } else {
            gDPSetCombineMode(dlHead++, G_CC_TEXTA, G_CC_TEXTA);
        }
    }

    gDPPipeSync(dlHead++);

    if (a == 0) {
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
        return; // No point in drawing invisible text!
    }

    if ((flags & FT_FLAG_ALIGN_MASK) != FT_FLAG_ALIGN_LEFT) {
        xShift = fasttext_calculate_width_of_line(string);
        if ((flags & FT_FLAG_ALIGN_MASK) == FT_FLAG_ALIGN_CENTER) {
            xShift = (xShift + 1) / 2;
        }
    }

    xPos -= xShift;

    create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, xPos, yPos, 0.0f);
    while (string[i] != '\0') {
        const s32 curChar = string[i];
        s32 xDist;
        s32 tabCount;

        switch (curChar) {
            case '\n': // newline
                yPos -= fontProps->lineHeight;
                if ((flags & FT_FLAG_ALIGN_MASK) != FT_FLAG_ALIGN_LEFT) {
                    xShift = fasttext_calculate_width_of_line(&string[i+1]);
                    if ((flags & FT_FLAG_ALIGN_MASK) == FT_FLAG_ALIGN_CENTER) {
                        xShift = (xShift + 1) / 2;
                    }
                }
                xPos = x - xShift;
                gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
                create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, xPos, yPos, 0.0f);
                break;
            case '\t': // tab
                xDist = xPos - (x - xShift);
                tabCount = (xDist + FT_TAB_WIDTH) / FT_TAB_WIDTH;
                xPos = tabCount * FT_TAB_WIDTH + (x - xShift);
                gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
                create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, xPos, yPos, 0.0f);
                break;
            case '@': // string color
                for (s32 j = 0; j < ARRAY_COUNT(rgbaCurrentColors); j++) {
                    if (j == 3) { // Alpha only
                        rgbaCurrentColors[j] = fasttext_string_to_byte_with_fallback(string[i+1], string[i+2], 255);
                        rgbaCurrentColors[j] = (((f32) rgbaBaseColors[j] * (f32) rgbaCurrentColors[j]) + 0.5f) / 255.0f;
                    } else {
                        rgbaCurrentColors[j] = fasttext_string_to_byte_with_fallback(string[i+1], string[i+2], rgbaBaseColors[j]);
                    }
                    i += 2;
                }
                gDPPipeSync(dlHead++);
                gDPSetPrimColor(dlHead++, 0, 0, rgbaCurrentColors[0], rgbaCurrentColors[1], rgbaCurrentColors[2], rgbaCurrentColors[3]);
                if (rgbaCurrentColors[3] == 255) {
                    gDPSetCombineMode(dlHead++, G_CC_TEXT, G_CC_TEXT);
                } else {
                    gDPSetCombineMode(dlHead++, G_CC_TEXTA, G_CC_TEXTA);
                }
                gDPPipeSync(dlHead++);
                break;
            case ' ':
                xPos += kerningTable[curChar - ' '];
                create_dl_translation_matrix(&dlHead, MENU_MTX_NOPUSH, kerningTable[curChar - ' '], 0.0f, 0.0f);
                break;
            default:
                xPos += kerningTable[curChar - ' '];
                slowtext_draw_char(&dlHead, fontProps, curChar);
                break;
        }

        i++;
    }

    gDPPipeSync(dlHead++);
    gDPSetPrimColor(dlHead++, 0, 0, rgbaBaseColors[0], rgbaBaseColors[1], rgbaBaseColors[2], rgbaBaseColors[3]);
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    gDisplayListHead = dlHead;
}

void slowtext_draw_ortho_text_linebreaks(s32 x, s32 y, s32 width, const char* string, enum FastTextFlags flags, s32 r, s32 g, s32 b, s32 a) {
    s32 lines = 0;
    s32 length = 0;
    fasttext_compute_print_text_with_line_breaks(fasttextCachedFontId, width, &lines, &length, gFasttextTmpBuffer, string);

    assert(length + 1 < (s32) sizeof(gFasttextTmpBuffer), "slowtext_draw_ortho_text_linebreaks:\nstring is too long!");
    slowtext_draw_ortho_text(x, y, gFasttextTmpBuffer, flags, r, g, b, a);
}

void slowtext_setup_ortho_rendering(enum FastTextFont fnt) {
    fasttextCachedFontId = fnt;
    if (fnt == FT_FONT_NONE) {
        slowtext_finished_rendering();
        return;
    }

    Gfx *dlHead = gDisplayListHead;
    const struct FastTextProps *fontProps = &gFasttextFonts[fasttextCachedFontId];

    gSPDisplayList(dlHead++, dl_fasttext_begin);
    gDPSetRenderMode(dlHead++, G_RM_XLU_SURF, G_RM_XLU_SURF);

    if (fontProps->textureFormat != G_IM_FMT_IA && fontProps->textureFormat == G_IM_FMT_I &&  fontProps->textureFormat == G_IM_FMT_RGBA) {
        error("slowtext textureFormat not supported!");
    }

    // NOTE: These switch cases are not nearly as stupid as they look. Due to the way these macros work, the `siz` argument cannot be set from a variable; it HAS to be set by the precompiler.
    // Creating a new macro for these would be possible, but it'd need 6 total fields in each struct entry for it (when just 1 suffices for this method).
    switch (fontProps->textureSize) {
        case G_IM_SIZ_4b:
            gDPLoadTextureBlock_4bS(dlHead++, fontProps->texture, fontProps->textureFormat, fontProps->textureWidth, fontProps->textureHeight, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            break;
        case G_IM_SIZ_8b:
            gDPLoadTextureBlockS(dlHead++, fontProps->texture, fontProps->textureFormat, G_IM_SIZ_8b, fontProps->textureWidth, fontProps->textureHeight, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            break;
        case G_IM_SIZ_16b:
            gDPLoadTextureBlockS(dlHead++, fontProps->texture, fontProps->textureFormat, G_IM_SIZ_16b, fontProps->textureWidth, fontProps->textureHeight, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            break;
        case G_IM_SIZ_32b:
            gDPLoadTextureBlockS(dlHead++, fontProps->texture, fontProps->textureFormat, G_IM_SIZ_32b, fontProps->textureWidth, fontProps->textureHeight, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            break;
        default:
            error("Invalid textureSize!");
    }

    gSPTexture(dlHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    bzero(slowtext_char_vtx, sizeof(slowtext_char_vtx));

    gDisplayListHead = dlHead;
}

void slowtext_finished_rendering(void) {
    Gfx *dlHead = gDisplayListHead;

    bzero(slowtext_char_vtx, sizeof(slowtext_char_vtx));
    gSPTexture(dlHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
    gSPDisplayList(dlHead++, dl_fasttext_end);

    fasttextCachedFontId = FT_FONT_NONE;

    gDisplayListHead = dlHead;
}
