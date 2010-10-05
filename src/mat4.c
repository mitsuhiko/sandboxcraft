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

sc_mat4_t *
sc_mat4_mul(sc_mat4_t *mat_out, const sc_mat4_t *mat1, const sc_mat4_t *mat2)
{
    const float *m1 = mat1->elms, *m2 = mat2->elms;

    mat_out->elms[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
    mat_out->elms[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
    mat_out->elms[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
    mat_out->elms[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

    mat_out->elms[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
    mat_out->elms[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
    mat_out->elms[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
    mat_out->elms[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

    mat_out->elms[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
    mat_out->elms[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
    mat_out->elms[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
    mat_out->elms[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

    mat_out->elms[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
    mat_out->elms[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
    mat_out->elms[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
    mat_out->elms[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

    return mat_out;
}

sc_mat4_t *
sc_mat4_scalar_mul(sc_mat4_t *mat_out, const sc_mat4_t *mat, float factor)
{
    int i;
    for (i = 0; i < 16; i++)
        mat_out->elms[i] = mat->elms[i] * factor;
    return mat_out;
}
