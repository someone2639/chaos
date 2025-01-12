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
extern char gFasttextTmpBuffer[0x1000];

/**
 * fasttext vs slowtext:
 * 
 * fasttext: Uses texrects to render text to the screen.
 *           fasttext is the more performant option and looks good on all plugins,
 *           however it requires absolute x/y coordinates and is not capable of printing to negative coordinates.
 * 
 * slowtext: Uses ortho tris to render text to the screen.
 *           slowtext is the less performant option and eats a significant portion of the Gfx buffer,
 *           however it may be used to print x/y coordinates relative to a matrix transformation.
 *           It is still more performant than the vanilla text engine though overall.
 *           slowtext is the preferred option for scalable, translatable, and rotatable text printing.
 * 
 * NOTE: Ortho tris and texrexts operate on inverse y coordinates!
 * fasttext: y=0 at the top of the screen
 * slowtext: y=0 at the bottom of the screen (or applicable transformation)
*/

/*********************************************************************
 * fasttext rendering: use these together for texrect printing only! *
 *********************************************************************/
// fasttext initialization function to be run before texrects can be drawn.
// This may be run without need for `fasttext_finished_rendering` when switching fonts.
// Do not attempt to render unrelated DisplayLists until `fasttext_finished_rendering` has been invoked.
void fasttext_setup_textrect_rendering(enum FastTextFont fnt);

// fasttext function used for physically drawing a message to the screen using texrects.
void fasttext_draw_texrect(int x, int y, const char* string, enum FastTextFlags flags, int r, int g, int b, int a);

// same as above, but autocomputes newlines based on a provided string width value.
void fasttext_draw_texrect_linebreaks(int x, int y, int width, const char* string, enum FastTextFlags flags, int r, int g, int b, int a);

// fasttext finalization function used when done rendering texrects.
// Do not forget to invoke this!
void fasttext_finished_rendering(void);


/***********************************************************************
 * slowtext rendering: use these together for ortho tri printing only! *
 ***********************************************************************/
// slowtext initialization function to be run before ortho tris can be drawn.
// This may be run without need for `slowtext_finished_rendering` when switching fonts.
// Do not attempt to render unrelated DisplayLists until `slowtext_finished_rendering` has been invoked.
void slowtext_setup_ortho_rendering(enum FastTextFont fnt);

// slowtext function used for physically drawing a message to the screen using ortho tris.
void slowtext_draw_ortho_text(s32 x, s32 y, const char* string, enum FastTextFlags flags, s32 r, s32 g, s32 b, s32 a);

// same as above, but autocomputes newlines based on a provided string width value.
void slowtext_draw_ortho_text_linebreaks(s32 x, s32 y, s32 width, const char* string, enum FastTextFlags flags, s32 r, s32 g, s32 b, s32 a);

// slowtext finalization function used when done rendering ortho tris.
// Do not forget to invoke this!
void slowtext_finished_rendering(void);


/*************************************************************************************
 * fasttext helper functions: can be used applicably with both fasttext and slowtext *
 *************************************************************************************/
// Internal fasttext function used for converting hexadecimal string data into binary data.
u8 fasttext_string_to_byte_with_fallback(u8 leftNibbleChar, u8 rightNibbleChar, u8 fallbackValue);

// Computes the number of total lines present in a string. 
int fasttext_calculate_line_count(const char* string);

// Computes the width of the first line in a string.
// Returns results up to the first newline character or NULL terminator.
int fasttext_calculate_width_of_line(const char* string);

// Automatically inserts line breaks into a destination string.
// Additionally writes to user-provided pointers describing new line count and string length.
void fasttext_compute_print_text_with_line_breaks(const enum FastTextFont font, const s32 lineWidth, s32 *numComputedLines, s32 *strComputedLength, char *dst, const char *src);

#endif
