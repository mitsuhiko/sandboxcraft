#include "sc_mat3.h"
#include "sc_math.h"

sc_mat3_t *
sc_mat3_set(sc_mat3_t *mat, float a, float b, float c, float d, float e,
            float f, float g, float h, float i)
{
    mat->m[0][0] = a;
    mat->m[0][1] = b;
    mat->m[0][2] = c;
    mat->m[1][0] = d;
    mat->m[1][1] = e;
    mat->m[1][2] = f;
    mat->m[2][0] = g;
    mat->m[2][1] = h;
    mat->m[2][2] = i;
    return mat;
}

sc_mat3_t *
sc_mat3_set_identity(sc_mat3_t *mat)
{
    memset(mat, 0, sizeof(sc_mat3_t));
    mat->m[0][0] = 1.0f;
    mat->m[1][1] = 1.0f;
    mat->m[2][2] = 1.0f;
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
    const float *m = mat->elms;

    mat_out->elms[0] = m[4] * m[8] - m[5] * m[7];
    mat_out->elms[1] = m[2] * m[7] - m[1] * m[8];
    mat_out->elms[2] = m[1] * m[5] - m[2] * m[4];
    mat_out->elms[3] = m[5] * m[6] - m[3] * m[8];
    mat_out->elms[4] = m[0] * m[8] - m[2] * m[6];
    mat_out->elms[5] = m[2] * m[3] - m[0] * m[5];
    mat_out->elms[6] = m[3] * m[7] - m[4] * m[6];
    mat_out->elms[7] = m[1] * m[6] - m[9] * m[7];
    mat_out->elms[8] = m[0] * m[4] - m[1] * m[3];

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
            mat_out->m[y][x] = mat->m[x][y];
    return mat_out;
}
