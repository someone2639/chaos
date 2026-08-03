#pragma once

#include <PR/ultratypes.h>

typedef struct {
    f32 h;
    f32 s;
    f32 v;
    f32 a;
} HSV;

typedef struct {
    union {
        // Individual color channels
        struct {
            u8 r;
            u8 g;
            u8 b;
            u8 a;
        } c;
        // Full rgba32 value     
        u32 f;
    };
} RGBA;

#define RGBA16_TO_RBGA32(rgba16, rgba32) \
    rgba32.c.r = (((rgba16 >> 11) & 31) * (255.0f / 31.0f)); \
    rgba32.c.g = (((rgba16 >> 6) & 31) * (255.0f / 31.0f)); \
    rgba32.c.b = (((rgba16 >> 1) & 31) * (255.0f / 31.0f)); \
    rgba32.c.a = ((rgba16) & 1) ? 255 : 0;

#define RGBA32_TO_RGBA16(rgba32, rgba16) \
    rgba16 = GPACK_RGBA5551(rgba32.c.r, rgba32.c.g, rgba32.c.b, rgba32.c.a)

RGBA HSV_to_RGB(HSV hsv);
HSV RGB_to_HSV(RGBA rgb);