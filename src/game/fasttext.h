#ifndef __FASTTEXT_H__
#define __FASTTEXT_H__

void drawSmallString_impl(Gfx**, int, int, const char*, int, int, int, int);

void fasttext_setup_rendering(Gfx **dl, u8 allowAlpha);

static inline void drawSmallString(Gfx **dl, int x, int y, const char* string) {
  drawSmallString_impl(dl, x, y, string, 255, 255, 255, 255);
}

static inline void drawSmallStringCol(Gfx **dl, int x, int y, const char* string, int r, int g, int b, int a) {
  drawSmallString_impl(dl, x, y, string, r, g, b, a);
}

#endif
