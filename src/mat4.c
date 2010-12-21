#include "sc_mat4.h"
#include "sc_math.h"

/* thank you windows header */
#undef near
#undef far

#define EPSILON (1e-6)

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
    mat->m[0][0] = 1.0f;
    mat->m[1][1] = 1.0f;
    mat->m[2][2] = 1.0f;
    mat->m[3][3] = 1.0f;
    return mat;
}

sc_mat4_t *
sc_mat4_set_perspective(sc_mat4_t *mat_out, float fovy, float aspect,
                        float near, float far)
{
    float f = 1.0f / tanf(fovy * (SC_PI / 360.0));
    float *m = mat_out->elms;

    m[0] = f / aspect;
    m[1] = 0.0;
    m[2] = 0.0;
    m[3] = 0.0;

    m[4] = 0.0;
    m[5] = f;
    m[6] = 0.0;
    m[7] = 0.0;

    m[8] = 0.0;
    m[9] = 0.0;
    m[10] = (near + far) / (near - far);
    m[11] = -1.0;

    m[12] = 0.0;
    m[13] = 0.0;
    m[14] = 2.0 * near * far / (near - far);
    m[15] = 0.0;

    return mat_out;
}

sc_mat4_t *
sc_mat4_set_translation(sc_mat4_t *mat_out, float x, float y, float z)
{
    sc_vec3_t vec = {x, y, z};
    return sc_mat4_set_translation_vector(mat_out, &vec);
}

sc_mat4_t *
sc_mat4_set_translation_vector(sc_mat4_t *mat_out, const sc_vec3_t *vec)
{
    sc_mat4_set_identity(mat_out);
    mat_out->elms[12] = vec->x;
    mat_out->elms[13] = vec->y;
    mat_out->elms[14] = vec->z;
    return mat_out;
}

sc_mat4_t *
sc_mat4_look_at(sc_mat4_t *mat_out, const sc_vec3_t *eye,
                const sc_vec3_t *forward, const sc_vec3_t *up)
{
    sc_vec3_t s, u, f, inverse_eye;
    sc_mat4_t translation;
    float *m = mat_out->elms;

    sc_vec3_normalize(&f, forward);
    sc_vec3_cross(&s, &f, up);
    sc_vec3_normalize(&s, &s);
    sc_vec3_cross(&u, &s, &f);

    m[0] = s.x;
    m[1] = u.x;
    m[2] = -f.x;
    m[3] = 0.0f;
    m[4] = s.y;
    m[5] = u.y;
    m[6] = -f.y;
    m[7] = 0.0f;
    m[8] = s.z;
    m[9] = u.z;
    m[10] = -f.z;
    m[11] = 0.0f;
    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;

    sc_vec3_neg(&inverse_eye, eye);
    sc_mat4_set_translation_vector(&translation, &inverse_eye);
    sc_mat4_mul(mat_out, &translation, mat_out);

    return mat_out;
}

sc_mat4_t *
sc_mat4_from_axis_rotation(sc_mat4_t *mat, float angle, float x, float y, float z)
{
    sc_vec3_t axis = {x, y, z};
    return sc_mat4_from_axis_rotation_vector(mat, angle, &axis);
}

sc_mat4_t *
sc_mat4_from_axis_rotation_vector(sc_mat4_t *mat, float angle, const sc_vec3_t *axis)
{
    sc_vec3_t vec;
    float c = cosf(sc_deg2rad(angle));
    float s = sinf(sc_deg2rad(angle));
    sc_vec3_normalize(&vec, axis);

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

#define MINOR(M, r0, r1, r2, c0, c1, c2) ( \
    M->m[r0][c0] * \
        (M->m[r1][c1] * M->m[r2][c2] - M->m[r2][c1] * M->m[r1][c2]) - \
    M->m[r0][c1] * \
        (M->m[r1][c0] * M->m[r2][c2] - M->m[r2][c0] * M->m[r1][c2]) + \
    M->m[r0][c2] * \
        (M->m[r1][c0] * M->m[r2][c1] - M->m[r2][c0] * M->m[r1][c1]))

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
    sc_mat4_t tmp;
    float v[6], t00, t10, t20, t30, det, idet, (*mo)[4] = tmp.m;
    const float (*mi)[4] = mat->m;

    v[0] = mi[2][0] * mi[3][1] - mi[2][1] * mi[3][0];
    v[1] = mi[2][0] * mi[3][2] - mi[2][2] * mi[3][0];
    v[2] = mi[2][0] * mi[3][3] - mi[2][3] * mi[3][0];
    v[3] = mi[2][1] * mi[3][2] - mi[2][2] * mi[3][1];
    v[4] = mi[2][1] * mi[3][3] - mi[2][3] * mi[3][1];
    v[5] = mi[2][2] * mi[3][3] - mi[2][3] * mi[3][2];

    t00 = + (v[5] * mi[1][1] - v[4] * mi[1][2] + v[3] * mi[1][3]);
    t10 = - (v[5] * mi[1][0] - v[2] * mi[1][2] + v[1] * mi[1][3]);
    t20 = + (v[4] * mi[1][0] - v[2] * mi[1][1] + v[0] * mi[1][3]);
    t30 = - (v[3] * mi[1][0] - v[1] * mi[1][1] + v[0] * mi[1][2]);

    det = (t00 * mi[0][0] + t10 * mi[0][1] + t20 * mi[0][2] + t30 * mi[0][3]);
    if (det == 0)
        return NULL;
    idet = 1.0f / det;

    mo[0][0] = t00 * idet;
    mo[1][0] = t10 * idet;
    mo[2][0] = t20 * idet;
    mo[3][0] = t30 * idet;

    mo[0][1] = - (v[5] * mi[0][1] - v[4] * mi[0][2] + v[3] * mi[0][3]) * idet;
    mo[1][1] = + (v[5] * mi[0][0] - v[2] * mi[0][2] + v[1] * mi[0][3]) * idet;
    mo[2][1] = - (v[4] * mi[0][0] - v[2] * mi[0][1] + v[0] * mi[0][3]) * idet;
    mo[3][1] = + (v[3] * mi[0][0] - v[1] * mi[0][1] + v[0] * mi[0][2]) * idet;

    v[0] = mi[1][0] * mi[3][1] - mi[1][1] * mi[3][0];
    v[1] = mi[1][0] * mi[3][2] - mi[1][2] * mi[3][0];
    v[2] = mi[1][0] * mi[3][3] - mi[1][3] * mi[3][0];
    v[3] = mi[1][1] * mi[3][2] - mi[1][2] * mi[3][1];
    v[4] = mi[1][1] * mi[3][3] - mi[1][3] * mi[3][1];
    v[5] = mi[1][2] * mi[3][3] - mi[1][3] * mi[3][2];

    mo[0][2] = + (v[5] * mi[0][1] - v[4] * mi[0][2] + v[3] * mi[0][3]) * idet;
    mo[1][2] = - (v[5] * mi[0][0] - v[2] * mi[0][2] + v[1] * mi[0][3]) * idet;
    mo[2][2] = + (v[4] * mi[0][0] - v[2] * mi[0][1] + v[0] * mi[0][3]) * idet;
    mo[3][2] = - (v[3] * mi[0][0] - v[1] * mi[0][1] + v[0] * mi[0][2]) * idet;

    v[0] = mi[2][1] * mi[1][0] - mi[2][0] * mi[1][1];
    v[1] = mi[2][2] * mi[1][0] - mi[2][0] * mi[1][2];
    v[2] = mi[2][3] * mi[1][0] - mi[2][0] * mi[1][3];
    v[3] = mi[2][2] * mi[1][1] - mi[2][1] * mi[1][2];
    v[4] = mi[2][3] * mi[1][1] - mi[2][1] * mi[1][3];
    v[5] = mi[2][3] * mi[1][2] - mi[2][2] * mi[1][3];

    mo[0][3] = - (v[5] * mi[0][1] - v[4] * mi[0][2] + v[3] * mi[0][3]) * idet;
    mo[1][3] = + (v[5] * mi[0][0] - v[2] * mi[0][2] + v[1] * mi[0][3]) * idet;
    mo[2][3] = - (v[4] * mi[0][0] - v[2] * mi[0][1] + v[0] * mi[0][3]) * idet;
    mo[3][3] = + (v[3] * mi[0][0] - v[1] * mi[0][1] + v[0] * mi[0][2]) * idet;

    *mat_out = tmp;
    return mat_out;
}

sc_mat4_t *
sc_mat4_mul(sc_mat4_t *mat_out, const sc_mat4_t *mat1, const sc_mat4_t *mat2)
{
    sc_mat4_t rv;
    int i, j, k;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++) {
            float sum = 0.0f;
            for (k = 0; k < 4; k++)
                sum += mat1->m[i][k] * mat2->m[k][j];
            rv.m[i][j] = sum;
        }

    *mat_out = rv;
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
    sc_mat4_t tmp = *mat;
    for (y = 0; y < 4; y++)
        for (x = 0; x < 4; x++)
            tmp.m[y][x] = mat->m[x][y];
    *mat_out = tmp;
    return mat_out;
}

int
sc_mat4_equal(const sc_mat4_t *mat1, const sc_mat4_t *mat2)
{
    int i;
    for (i = 0; i < 16; i++)
        if (!(mat1->elms[i] < mat2->elms[i] + EPSILON &&
              mat1->elms[i] > mat2->elms[i] - EPSILON))
            return 0;
    return 1;
}
