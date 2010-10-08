/* implements a basic color abstraction */
#ifndef _INC_SC_COLOR_H_
#define _INC_SC_COLOR_H_

typedef unsigned char sc_color_component;
typedef struct {
    sc_color_component r;
    sc_color_component g;
    sc_color_component b;
    sc_color_component a;
} sc_color_t;

sc_color_t sc_color(int val);

sc_color_t sc_make_color(sc_color_component r, sc_color_component g,
                         sc_color_component b, sc_color_component a);

void sc_color_to_float(sc_color_t color, float *r, float *g, float *b,
                       float *a);

#endif
