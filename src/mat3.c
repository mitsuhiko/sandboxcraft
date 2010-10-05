#include <math.h>

#include "sc_mat3.h"

#define EPSILON (1e-6)

sc_mat3_t *
sc_mat3_set(sc_mat3_t *mat, float a, float b, float c, float d, float e,
            float f, float g, float h, float i)
{
    sc_mat3(mat, 0, 0) = a;
    sc_mat3(mat, 0, 1) = b;
    sc_mat3(mat, 0, 2) = c;
    sc_mat3(mat, 1, 0) = d;
    sc_mat3(mat, 1, 1) = e;
    sc_mat3(mat, 1, 2) = f;
    sc_mat3(mat, 2, 0) = g;
    sc_mat3(mat, 2, 1) = h;
    sc_mat3(mat, 2, 2) = i;
    return mat;
}

sc_mat3_t *
sc_mat3_set_identity(sc_mat3_t *mat)
{
    memset(sc_mat3_ptr(mat), 0, sizeof(sc_mat3_t));
    sc_mat3(mat, 0, 0) = 1.0f;
    sc_mat3(mat, 1, 1) = 1.0f;
    sc_mat3(mat, 2, 2) = 1.0f;
    return mat;
}

float
sc_mat3_determinant(sc_mat3_t *mat)
{
    return (mat->elms[0] * mat->elms[4] * mat->elms[8]
          + mat->elms[1] * mat->elms[5] * mat->elms[6]
          + mat->elms[2] * mat->elms[3] * mat->elms[7])
         - (mat->elms[2] * mat->elms[4] * mat->elms[6]
          + mat->elms[0] * mat->elms[5] * mat->elms[7]
          + mat->elms[1] * mat->elms[3] * mat->elms[8]);
}
