#include "sc_color.h"

sc_color_t
sc_color(int val)
{
    sc_color_t rv;
    rv.r = val >> 24;
    rv.g = (val >> 16) & 0xff;
    rv.b = (val >> 8) & 0xff;
    rv.a = val & 0xff;
    return rv;
}

sc_color_t
sc_make_color(sc_color_component r, sc_color_component g,
              sc_color_component b, sc_color_component a)
{
    sc_color_t color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

void
sc_color_to_float(sc_color_t color, float *r, float *g, float *b, float *a)
{
    *r = color.r / 255.0f;
    *g = color.g / 255.0f;
    *b = color.b / 255.0f;
    *a = color.a / 255.0f;
}

void
sc_color_to_floatv(sc_color_t color, float *f)
{
    sc_color_to_float(color, &f[0], &f[1], &f[2], &f[3]);
}
