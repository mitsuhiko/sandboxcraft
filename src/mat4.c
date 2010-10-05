#include <math.h>

#include "sc_mat4.h"

sc_mat4_t *
sc_mat4_set(sc_mat4_t *mat, float a, float b, float c, float d, float e,
            float f, float g, float h, float i, float j, float k, float l,
            float m, float n, float o, float p)
{
    sc_mat4(mat, 0, 0) = a;
    sc_mat4(mat, 0, 1) = b;
    sc_mat4(mat, 0, 2) = c;
    sc_mat4(mat, 0, 3) = d;
    sc_mat4(mat, 1, 0) = e;
    sc_mat4(mat, 1, 1) = f;
    sc_mat4(mat, 1, 2) = g;
    sc_mat4(mat, 1, 3) = h;
    sc_mat4(mat, 2, 0) = i;
    sc_mat4(mat, 2, 1) = j;
    sc_mat4(mat, 2, 2) = k;
    sc_mat4(mat, 2, 3) = l;
    sc_mat4(mat, 3, 0) = m;
    sc_mat4(mat, 3, 1) = n;
    sc_mat4(mat, 3, 2) = o;
    sc_mat4(mat, 3, 3) = p;
    return mat;
}

sc_mat4_t *
sc_mat4_set_identity(sc_mat4_t *mat)
{
    memset(mat, 0, sizeof(sc_mat4_t));
    sc_mat4(mat, 0, 0) = 1.0f;
    sc_mat4(mat, 1, 1) = 1.0f;
    sc_mat4(mat, 2, 2) = 1.0f;
    sc_mat4(mat, 3, 3) = 1.0f;
    return mat;
}
