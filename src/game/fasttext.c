// This file is a modification of a file from https://github.com/danbolt/n64-jam-1, which was licensed under the MPL-2.0 License
// See the original repo for more details.

#include <ultra64.h>
#include "macros.h"
#include "segment2.h"
#include "camera.h"
#include "debug.h"
#include "game_init.h"
#include "fasttext.h"

#define TEX_ASCII_START '!'

#define G_CC_TEXT PRIMITIVE, 0, TEXEL0, 0, 0, 0, 0, TEXEL0
#define G_CC_TEXTA TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0

enum FastTextFont fasttextCachedFontId = FT_FONT_NONE;

char gFasttextTmpBuffer[0x1000];

#include "fasttext_data.c.in"

u8 fasttext_string_to_byte_with_fallback(u8 leftNibbleChar, u8 rightNibbleChar, u8 fallbackValue) {
    u8 ret = 0;

    if (leftNibbleChar >= '0' && leftNibbleChar <= '9') {
        ret |= (((leftNibbleChar - '0') & 0x0F) << 4);
    } else if (leftNibbleChar >= 'A' && leftNibbleChar <= 'F') {
        ret |= (((leftNibbleChar - 'A') + 0x0A) & 0x0F) << 4;
    } else if (leftNibbleChar >= 'a' && leftNibbleChar <= 'f') {
        ret |= (((leftNibbleChar - 'a') + 0x0A) & 0x0F) << 4;
    } else {
        assert(leftNibbleChar == '-', "Invalid slowtext string color!");
        ret |= (fallbackValue & 0xF0);
    }

    if (rightNibbleChar >= '0' && rightNibbleChar <= '9') {
        ret |= ((rightNibbleChar - '0') & 0x0F);
    } else if (rightNibbleChar >= 'A' && rightNibbleChar <= 'F') {
        ret |= ((rightNibbleChar - 'A') + 0x0A) & 0x0F;
    } else if (rightNibbleChar >= 'a' && rightNibbleChar <= 'f') {
        ret |= ((rightNibbleChar - 'a') + 0x0A) & 0x0F;
    } else {
        assert(rightNibbleChar == '-', "Invalid slowtext string color!");
        ret |= (fallbackValue & 0x0F);
    }

    return ret;
}

int fasttext_calculate_line_count(const char* string) {
    int lineCount = 1;
    int i = 0;

    if (!string) {
        assert(FALSE, "Attempted to pass NULL string to fasttext line count func!");
        return 1;
    }

    while (string[i] != '\0') {
        if (string[i] == '\n') {
            lineCount++;
        }

        i++;
    }

    return lineCount;
}

int fasttext_calculate_width_of_line(const char* string) {
    int xPos = 0;
    int i = 0;

    if (!string) {
        assert(FALSE, "Attempted to pass NULL string to fasttext line width func!");
        return 0;
    }

    aggress(fasttextCachedFontId != FT_FONT_NONE, "Neglected fasttext setup before checking width");

    const struct FastTextProps *fontProps = &gFasttextFonts[fasttextCachedFontId];
    const unsigned char *kerningTable = fontProps->kerningTable;

    while (string[i] != '\0' && string[i] != '\n') {
        int curChar = string[i];
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

    return xPos;
}

void fasttext_compute_print_text_with_line_breaks(const enum FastTextFont font, const s32 lineWidth, s32 *numComputedLines, s32 *strComputedLength, char *dst, const char *src) {
    const s32 newLineWidth = ABS(lineWidth);

    aggress(font != FT_FONT_NONE, "fasttext_compute_print_text_with_line_breaks:\nPassed in invalid font!");

    if (!dst) {
        assert(FALSE, "fasttext_compute_print_text_with_line_breaks:\nAttempted to pass NULL dst pointer!");
        return;
    }

    if (!src) {
        assert(FALSE, "fasttext_compute_print_text_with_line_breaks:\nAttempted to pass NULL src pointer!");
        return;
    }

    if (newLineWidth == 0) {
        assert(FALSE, "fasttext_compute_print_text_with_line_breaks:\nAttempted to pass 0 or negative line width!");
    }

    const struct FastTextProps *props = &gFasttextFonts[font];
    const unsigned char *kerningTable = props->kerningTable;

    s32 width = 0;
    s32 lastWidth = 0;
    s32 parseIndex = 0;
    s32 fillStrIndex = 0;
    s32 lastFilledParseIndex = 0;
    u8 automaticNewline = FALSE;
    *numComputedLines = 1;

    do {
        s32 curChar = src[parseIndex];
        s32 charIndex = curChar - ' ';
        s32 tabCount;

        switch (curChar) {
            case '@':
                // NOTE: Do not mess up color codes, or this could crash easily!
                parseIndex += 8;
                break;

            case '\t':    
                tabCount = (width + FT_TAB_WIDTH) / FT_TAB_WIDTH;
                width = tabCount * FT_TAB_WIDTH;
                break;

            case '\n':
            case ' ':
            case '\0':
                if (width >= newLineWidth) {
                    width -= lastWidth;

                    for (; fillStrIndex > 0; fillStrIndex--) {
                        if (dst[fillStrIndex - 1] != ' ') {
                            break;
                        }
                    }

                    for (; lastFilledParseIndex < parseIndex; lastFilledParseIndex++) {
                        if (src[lastFilledParseIndex] != ' ') {
                            break;
                        }

                        width -= kerningTable[src[lastFilledParseIndex] - ' '];
                    }

                    if (width < 0) {
                        width = 0;
                    }

                    (*numComputedLines)++;
                    dst[fillStrIndex++] = '\n';
                    automaticNewline = TRUE;
                }
                
                while (lastFilledParseIndex < parseIndex) {
                    dst[fillStrIndex++] = src[lastFilledParseIndex++];
                }

                lastWidth = width;
                if (curChar == '\n') {
                    dst[fillStrIndex++] = curChar;
                    lastFilledParseIndex++;
                    (*numComputedLines)++;
                    width = 0;
                    lastWidth = 0;
                    automaticNewline = FALSE;
                } else if (curChar == ' ') {
                    if (width == 0 && automaticNewline) {
                        lastFilledParseIndex++;
                    } else {
                        width += kerningTable[charIndex];
                    }
                }

                break;

            default:
                width += kerningTable[charIndex];
                break;
        }

    } while (src[parseIndex++] != '\0');

    dst[fillStrIndex] = '\0';
    *strComputedLength = fillStrIndex;
}

void fasttext_draw_texrect(int x, int y, const char* string, enum FastTextFlags flags, int r, int g, int b, int a) {
    int i = 0;
    int xShift = 0;
    int xPos = x;
    int yPos;
    int s = 0;
    Gfx *dlHead = gDisplayListHead;
    unsigned char rgbaBaseColors[4] = { r, g, b, a };
    unsigned char rgbaCurrentColors[4] = { r, g, b, a };

    if (!string) {
        assert(FALSE, "Attempted to pass NULL string to fasttext!");
        return;
    }

    aggress(fasttextCachedFontId != FT_FONT_NONE, "Neglected fasttext setup");

    const struct FastTextProps *fontProps = &gFasttextFonts[fasttextCachedFontId];
    const unsigned char *kerningTable = fontProps->kerningTable;
    const unsigned int renderHeight = fontProps->textureHeight;
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
        return; // No point in drawing invisible text!
    }

    if ((flags & FT_FLAG_ALIGN_MASK) != FT_FLAG_ALIGN_LEFT) {
        xShift = fasttext_calculate_width_of_line(string);
        if ((flags & FT_FLAG_ALIGN_MASK) == FT_FLAG_ALIGN_CENTER) {
            xShift = (xShift + 1) / 2;
        }
    }

    xPos -= xShift;

    while (string[i] != '\0') {
        int curChar = string[i];
        int charIndex = curChar - ' ';
        int xDist;
        int tabCount;

        switch (curChar) {
            case '\n': // newline
                yPos += fontProps->lineHeight;
                if ((flags & FT_FLAG_ALIGN_MASK) != FT_FLAG_ALIGN_LEFT) {
                    xShift = fasttext_calculate_width_of_line(&string[i+1]);
                    if ((flags & FT_FLAG_ALIGN_MASK) == FT_FLAG_ALIGN_CENTER) {
                        xShift = (xShift + 1) / 2;
                    }
                }
                xPos = x - xShift;
                break;
            case '\t': // tab
                xDist = xPos - (x - xShift);
                tabCount = (xDist + FT_TAB_WIDTH) / FT_TAB_WIDTH;
                xPos = tabCount * FT_TAB_WIDTH + (x - xShift);
                break;
            case '@': // string color
                for (int j = 0; j < ARRAY_COUNT(rgbaCurrentColors); j++) {
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
                xPos += kerningTable[charIndex];
                break;
            default:
                s = fontProps->offsetTable[charIndex].horizontalOffset << 5;
                if ((s & (0x200 - 1)) == 0) {
                    s++;
                }

                // NOTE: Be careful nothing goes negative with text alignment! (Not using gSPScisTextureRectangle)
                gSPTextureRectangle(dlHead++, (xPos + 0) << 2, (yPos + 0) << 2, (xPos + fontProps->offsetTable[charIndex].renderWidth) << 2, (yPos + renderHeight) << 2, 0, s, 0, 1 << 10, 1 << 10);
                xPos += kerningTable[charIndex];
                break;
        }

        i++;
    }

    gDPPipeSync(dlHead++);

    gDisplayListHead = dlHead;
}

void fasttext_draw_texrect_linebreaks(int x, int y, int width, const char* string, enum FastTextFlags flags, int r, int g, int b, int a) {
    int lines = 0;
    int length = 0;
    fasttext_compute_print_text_with_line_breaks(fasttextCachedFontId, width, &lines, &length, gFasttextTmpBuffer, string);

    assert(length + 1 < (s32) sizeof(gFasttextTmpBuffer), "fasttext_draw_texrect_linebreaks:\nstring is too long!");
    fasttext_draw_texrect(x, y, gFasttextTmpBuffer, flags, r, g, b, a);
}

void fasttext_setup_textrect_rendering(enum FastTextFont fnt) {
    fasttextCachedFontId = fnt;
    if (fnt == FT_FONT_NONE) {
        fasttext_finished_rendering();
        return;
    }

    Gfx *dlHead = gDisplayListHead;
    const struct FastTextProps *fontProps = &gFasttextFonts[fasttextCachedFontId];

    gSPDisplayList(dlHead++, dl_fasttext_begin);
    gDPSetTexturePersp(dlHead++, G_TP_NONE);
    gDPSetRenderMode(dlHead++, G_RM_XLU_SURF, G_RM_XLU_SURF);

    if (fontProps->textureFormat != G_IM_FMT_IA && fontProps->textureFormat == G_IM_FMT_I &&  fontProps->textureFormat == G_IM_FMT_RGBA) {
        error("fasttext textureFormat not supported!");
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

    gDisplayListHead = dlHead;
}

void fasttext_finished_rendering(void) {
    Gfx *dlHead = gDisplayListHead;

    gDPSetTexturePersp(dlHead++, G_TP_PERSP);
    gSPDisplayList(dlHead++, dl_fasttext_end);

    fasttextCachedFontId = FT_FONT_NONE;

    gDisplayListHead = dlHead;
}
