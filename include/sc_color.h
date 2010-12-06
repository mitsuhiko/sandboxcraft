/* implements a basic color abstraction */
#ifndef _INC_SC_COLOR_H_
#define _INC_SC_COLOR_H_

#include "sc_boot.h"

typedef unsigned char sc_color_component;
typedef struct {
    sc_color_component r;
    sc_color_component g;
    sc_color_component b;
    sc_color_component a;
} sc_color_t;

/* returns a color from a 4 bit integer */
sc_color_t sc_color(uint32_t val);

/* makes a color from the four components in the range 0-255 */
sc_color_t sc_make_color(sc_color_component r, sc_color_component g,
                         sc_color_component b, sc_color_component a);

/* makes a color from an array of 4 floats */
sc_color_t sc_color_from_floatv(float *a);

/* converts a color to four floats */
void sc_color_to_float(sc_color_t color, float *r, float *g, float *b,
                       float *a);

/* converts a color into an array of four floats */
void sc_color_to_floatv(sc_color_t color, float *f);

#endif
