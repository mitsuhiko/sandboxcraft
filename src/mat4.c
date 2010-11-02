#include "sc_mat4.h"
#include "sc_math.h"

sc_mat4_t *
sc_mat4_set(sc_mat4_t *mat, float a, float b, float c, float d, float e,
            float f, float g, float h, float i, float j, float k, float l,
            float m, float n, float o, float p)
{
    mat->elms[0]  = a;
    mat->elms[1]  = b;
    mat->elms[2]  = c;
    mat->elms[3]  = d;
    mat->elms[4]  = e;
    mat->elms[5]  = f;
    mat->elms[6]  = g;
    mat->elms[7]  = h;
    mat->elms[8]  = i;
    mat->elms[9]  = j;
    mat->elms[10] = k;
    mat->elms[11] = l;
    mat->elms[12] = m;
    mat->elms[13] = n;
    mat->elms[14] = o;
    mat->elms[15] = p;
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
sc_mat4_from_axis_rotation(sc_mat4_t *mat, float angle, const sc_vec3_t *axis)
{
    float c = cosf(sc_deg2rad(angle));
    float s = sinf(sc_deg2rad(angle));
    sc_vec3_t vec = *axis;
    sc_vec3_normalize(&vec);

    mat->elms[0]  = c + vec.x * vec.x * (1 - c);
    mat->elms[1]  = vec.z * s + vec.y * vec.x * (1 - c);
    mat->elms[2]  = -vec.y * s + vec.z * vec.x * (1 - c);
    mat->elms[3]  = 0.0f;

    mat->elms[4]  = -vec.z * s + vec.x * vec.y * (1 - c);
    mat->elms[5]  = c + vec.y * vec.y * (1 - c);
    mat->elms[6]  = vec.x * s + vec.z * vec.y * (1 - c);
    mat->elms[7]  = 0.0f;

    mat->elms[8]  = vec.y * s + vec.x * vec.z * (1 - c);
    mat->elms[9]  = -vec.x * s + vec.y * vec.z * (1 - c);
    mat->elms[10] = c + vec.z * vec.z * (1 - c);
    mat->elms[11] = 0.0f;

    mat->elms[12] = 0.0f;
    mat->elms[13] = 0.0f;
    mat->elms[14] = 0.0f;
    mat->elms[15] = 1.0f;

    return mat;
}

#define MINOR(m, r0, r1, r2, c0, c1, c2) ( \
    sc_mat4(m, r0, c0) * \
        (sc_mat4(m, r1, c1) * sc_mat4(m, r2, c2) - \
         sc_mat4(m, r2, c1) * sc_mat4(m, r1, c2)) - \
    sc_mat4(m, r0, c1) * \
        (sc_mat4(m, r1, c0) * sc_mat4(m, r2, c2) - \
         sc_mat4(m, r2, c0) * sc_mat4(m, r1, c2)) + \
    sc_mat4(m, r0, c2) * \
        (sc_mat4(m, r1, c0) * sc_mat4(m, r2, c1) - \
         sc_mat4(m, r2, c0) * sc_mat4(m, r1, c1)))

sc_mat4_t *
sc_mat4_adjugate(sc_mat4_t *mat_out, const sc_mat4_t *mat)
{
    mat_out->elms[0]  =  MINOR(mat, 1, 2, 3, 1, 2, 3);
    mat_out->elms[1]  = -MINOR(mat, 0, 2, 3, 1, 2, 3);
    mat_out->elms[2]  =  MINOR(mat, 0, 1, 3, 1, 2, 3);
    mat_out->elms[3]  = -MINOR(mat, 0, 1, 2, 1, 2, 3);

    mat_out->elms[4]  = -MINOR(mat, 1, 2, 3, 0, 2, 3);
    mat_out->elms[5]  =  MINOR(mat, 0, 2, 3, 0, 2, 3);
    mat_out->elms[6]  = -MINOR(mat, 0, 1, 3, 0, 2, 3);
    mat_out->elms[7]  =  MINOR(mat, 0, 1, 2, 0, 2, 3);

    mat_out->elms[8]  =  MINOR(mat, 1, 2, 3, 0, 1, 3);
    mat_out->elms[9]  = -MINOR(mat, 0, 2, 3, 0, 1, 3);
    mat_out->elms[10] =  MINOR(mat, 0, 1, 3, 0, 1, 3);
    mat_out->elms[11] = -MINOR(mat, 0, 1, 2, 0, 1, 3);

    mat_out->elms[12] = -MINOR(mat, 1, 2, 3, 0, 1, 2);
    mat_out->elms[13] =  MINOR(mat, 0, 2, 3, 0, 1, 2);
    mat_out->elms[14] = -MINOR(mat, 0, 1, 3, 0, 1, 2);
    mat_out->elms[15] =  MINOR(mat, 0, 1, 2, 0, 1, 2);

    return mat_out;
}

float
sc_mat4_determinant(const sc_mat4_t *mat)
{
    return (
        mat->elms[0] * MINOR(mat, 1, 2, 3, 1, 2, 3) -
        mat->elms[1] * MINOR(mat, 1, 2, 3, 0, 2, 3) +
        mat->elms[2] * MINOR(mat, 1, 2, 3, 0, 1, 3) -
        mat->elms[3] * MINOR(mat, 1, 2, 3, 0, 1, 2)
    );
}

sc_mat4_t *
sc_mat4_inverse(sc_mat4_t *mat_out, const sc_mat4_t *mat)
{
    float t[4];
    sc_mat4_t tmp;
    int i, i1, j, k;

    tmp = *mat;
    sc_mat4_set_identity(mat_out);

    /* find largest pivot in column i among rosws i..3 */
    for (i = 0; i < 4; i++) {

        /* row with largest pivot candiate */
        i1 = i;
        for (j = i + 1; j < 4; j++)
            if (fabs(tmp.elms[j * 4 + i]) > fabs(tmp.elms[i1 * 4 + i]))
                i1 = j;

        /* swap rows i1 and i in a and b to put pivot on diagonal */
        for (k = 0; k < 4; k++)
            t[k] = tmp.elms[i1 * 4 + k];
        for (k = 0; k < 4; k++) {
            tmp.elms[i1 * 4 + k] = tmp.elms[i * 4 + k];
            tmp.elms[i * 4 + k] = t[k];
        }
        for (k = 0; k < 4; k++)
            t[k] = mat_out->elms[i1 * 4 + k];
        for (k = 0; k < 4; k++) {
            mat_out->elms[i1 * 4 + k] = mat_out->elms[i * 4 + k];
            mat_out->elms[i * 4 + k] = t[k];
        }

        /* scale row j to have a unit diagonal */
        if (!tmp.elms[i * 4 + i])
            return NULL;
        for (k = 0; k < 4; k++) {
            mat_out->elms[i * 4 + k] /= tmp.elms[i * 4 + i];
            tmp.elms[i * 4 + k] /= tmp.elms[i * 4 + i];
        }

        /* eliminate off-diagonal elements in col i of a, doing indentical
           operations to b */
        for (j = 0; j < 4; j++)
            if (i != j)
                for (k = 0; k < 4; k++) {
                    mat_out->elms[j * 4 + k] -= tmp.elms[j * 4 + i] *
                                                mat_out->elms[i * 4 + k];
                    tmp.elms[j * 4 + k] -= tmp.elms[j * 4 + i] *
                                           tmp.elms[i * 4 + k];
                }
    }

    return mat_out;
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

sc_mat4_t *
sc_mat4_transpose(sc_mat4_t *mat_out, const sc_mat4_t *mat)
{
    int x, y;
    for (y = 0; y < 4; y++)
        for (x = 0; x < 4; x++)
            sc_mat4(mat_out, y, x) = sc_mat4(mat, x, y);
    return mat_out;
}
