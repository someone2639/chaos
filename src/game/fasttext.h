#ifndef __FASTTEXT_H__
#define __FASTTEXT_H__

#define FASTTEXT_CHAR_COUNT (128 - ' ')
#define FT_TAB_WIDTH 16

enum FastTextFont {
    FT_FONT_NONE = -1,
    FT_FONT_OUTLINE,
    FT_FONT_MEDIUM,
    FT_FONT_SMALL_BOLD,
    FT_FONT_SMALL_THIN,
    FT_FONT_VANILLA_SHADOW,
    FT_FONT_LARGE,
    FT_FONT_HEADING,
    FT_FONT_SMALLHEX,
    FT_FONT_TINYHEX,

    FT_FONT_COUNT,
};

enum FastTextFlags {
    FT_FLAG_ALIGN_LEFT      = (0 << 0),
    FT_FLAG_ALIGN_CENTER    = (1 << 0),
    FT_FLAG_ALIGN_RIGHT     = (2 << 0),
    FT_FLAG_ALIGN_MASK      = (0b11 << 0),

    FT_FLAG_SKIP_COMBINER   = (1 << 2),
};

struct OffsetProps {
    u16 horizontalOffset;
    u8 renderWidth;
};

struct FastTextProps {
    const Texture *texture;
    const u8 *kerningTable;
    const struct OffsetProps *offsetTable;
    const u8 textureFormat;
    const u8 textureSize;
    const s8 yShift;
    const s8 subFromLength;
    const u16 textureWidth;
    const u8 textureHeight;
    const u8 lineHeight;
    const u8 averageCharHeight;
};

extern const struct FastTextProps gFasttextFonts[FT_FONT_COUNT];
extern enum FastTextFont fasttextCachedFontId;

u8 fasttext_string_to_byte_with_fallback(u8 leftNibbleChar, u8 rightNibbleChar, u8 fallbackValue);
int fasttext_calculate_line_count(const char* string);
int fasttext_calculate_width_of_line(const char* string);

void fasttext_setup_textrect_rendering(enum FastTextFont fnt);
void fasttext_draw_texrect(int x, int y, const char* string, enum FastTextFlags flags, int r, int g, int b, int a);
void fasttext_finished_rendering(void);

void slowtext_setup_ortho_rendering(enum FastTextFont fnt);
void slowtext_draw_ortho_text(s32 x, s32 y, const char* string, enum FastTextFlags flags, s32 r, s32 g, s32 b, s32 a);
void slowtext_finished_rendering(void);

#endif
