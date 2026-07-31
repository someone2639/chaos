#include "colors.h"
#include "engine/math_util.h"

/*
    Returns an RBG value from a HSV input
*/
RGBA HSV_to_RGB(HSV hsv) {
    f32 r = 0, g = 0, b = 0;

    if(hsv.s == 0) {
        r = hsv.v;
        g = hsv.v;
        b = hsv.v;
    }
    else {
        s16 i;
        f32 f, p, q, t;

        if(hsv.h == 360){
            hsv.h = 0;
        }
        else {
            hsv.h /= 60;
        }

        i = (s16)hsv.h;
        f = hsv.h - i;

        p = hsv.v * (1.0f - hsv.s);
        q = hsv.v * (1.0f - (hsv.s * f));
        t = hsv.v * (1.0f - (hsv.s * (1.0 - f)));

        switch (i) {
            case 0:
                r = hsv.v;
                g = t;
                b = p;
                break;
            case 1:
                r = q;
                g = hsv.v;
                b = p;
                break;
            case 2:
                r = p;
                g = hsv.v;
                b = t;
                break;
            case 3:
                r = p;
                g = q;
                b = hsv.v;
                break;
            case 4:
                r = t;
                g = p;
                b = hsv.v;
                break;
            default:
                r = hsv.v;
                g = p;
                b = q;
                break;
        }
    }

    RGBA rgb;
    rgb.c.r = 255 * r;
    rgb.c.g = 255 * g;
    rgb.c.b = 255 * b;
    rgb.c.a = 255 * hsv.a;

    return rgb;
}

/*
    Returns a HSV value from an RGB input
*/
HSV RGB_to_HSV(RGBA rgb) {
    HSV hsv;
    f32 r = rgb.c.r / 255.0f;
    f32 g = rgb.c.g / 255.0f;
    f32 b = rgb.c.b / 255.0f;
    f32 a = rgb.c.a / 255.0f;

    hsv.a = a;

    f32 max = MAX(r, MAX(g, b));
    f32 min = MIN(r, MIN(g, b));
    f32 diff = max - min;

    //Set value
    hsv.v = max;

    if (diff < 0.00001f) {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    if (max > 0.0f) {
        //Set saturation
        hsv.s = (diff / max);
    } else {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    //Set hue
    if(r >= max) {
        hsv.h = (g - b) / diff;
    } else if (g >= max) {
        hsv.h = 2.0f + ((b - r) / diff);
    } else {
        hsv.h = 4.0f + ((r - g) / diff);
    }

    hsv.h *= 60.0f;

    if (hsv.h < 0.0f) {
        hsv.h += 360.0f;
    }

    return hsv;
}