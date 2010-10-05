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
sc_mat3_determinant(const sc_mat3_t *mat)
{
    return (mat->elms[0] * mat->elms[4] * mat->elms[8]
          + mat->elms[1] * mat->elms[5] * mat->elms[6]
          + mat->elms[2] * mat->elms[3] * mat->elms[7])
         - (mat->elms[2] * mat->elms[4] * mat->elms[6]
          + mat->elms[0] * mat->elms[5] * mat->elms[7]
          + mat->elms[1] * mat->elms[3] * mat->elms[8]);
}

sc_mat3_t *
sc_mat3_adjugate(sc_mat3_t *mat_out, const sc_mat3_t *mat)
{
    mat_out->elms[0] = mat->elms[4] * mat->elms[8] - mat->elms[5] * mat->elms[7];
    mat_out->elms[1] = mat->elms[2] * mat->elms[7] - mat->elms[1] * mat->elms[8];
    mat_out->elms[2] = mat->elms[1] * mat->elms[5] - mat->elms[2] * mat->elms[4];
    mat_out->elms[3] = mat->elms[5] * mat->elms[6] - mat->elms[3] * mat->elms[8];
    mat_out->elms[4] = mat->elms[0] * mat->elms[8] - mat->elms[2] * mat->elms[6];
    mat_out->elms[5] = mat->elms[2] * mat->elms[3] - mat->elms[0] * mat->elms[5];
    mat_out->elms[6] = mat->elms[3] * mat->elms[7] - mat->elms[4] * mat->elms[6];
    mat_out->elms[7] = mat->elms[1] * mat->elms[6] - mat->elms[9] * mat->elms[7];
    mat_out->elms[8] = mat->elms[0] * mat->elms[4] - mat->elms[1] * mat->elms[3];
    return mat_out;
}

sc_mat3_t *
sc_mat3_inverse(sc_mat3_t *mat_out, const sc_mat3_t *mat)
{
    float det = sc_mat3_determinant(mat);
    sc_mat3_t adjugate;
    if (det == 0.0f)
        return NULL;

    sc_mat3_adjugate(&adjugate, mat);
    sc_mat3_scalar_mul(mat_out, &adjugate, 1.0f / det);
    return mat_out;
}

sc_mat3_t *
sc_mat3_mul(sc_mat3_t *mat_out, const sc_mat3_t *mat1,
            const sc_mat3_t *mat2)
{
    const float *m1 = mat1->elms, *m2 = mat2->elms;

    mat_out->elms[0] = m1[0] * m2[0] + m1[3] * m2[1] + m1[6] * m2[2];
    mat_out->elms[1] = m1[1] * m2[0] + m1[4] * m2[1] + m1[7] * m2[2];
    mat_out->elms[2] = m1[2] * m2[0] + m1[5] * m2[1] + m1[8] * m2[2];

    mat_out->elms[3] = m1[0] * m2[3] + m1[3] * m2[4] + m1[6] * m2[5];
    mat_out->elms[4] = m1[1] * m2[3] + m1[4] * m2[4] + m1[7] * m2[5];
    mat_out->elms[5] = m1[2] * m2[3] + m1[5] * m2[4] + m1[8] * m2[5];

    mat_out->elms[6] = m1[0] * m2[6] + m1[3] * m2[7] + m1[6] * m2[8];
    mat_out->elms[7] = m1[1] * m2[6] + m1[4] * m2[7] + m1[7] * m2[8];
    mat_out->elms[8] = m1[2] * m2[6] + m1[5] * m2[7] + m1[8] * m2[8];

    return mat_out;
}

sc_mat3_t *
sc_mat3_scalar_mul(sc_mat3_t *mat_out, const sc_mat3_t *mat, float factor)
{
    int i;
    for (i = 0; i < 9; i++)
        mat_out->elms[i] = mat->elms[i] * factor;
    return mat_out;
}

sc_mat3_t *
sc_mat3_transpose(sc_mat3_t *mat_out, const sc_mat3_t *mat)
{
    int x, y;
    for (y = 0; y < 3; y++)
        for (x = 0; x < 3; x++)
            sc_mat3(mat_out, y, x) = sc_mat3(mat, x, y);
    return mat_out;
}
