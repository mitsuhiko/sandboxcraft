#include "sc_vec3.h"
#include "sc_vec4.h"
#include "sc_mat4.h"
#include "sc_math.h"

#define EPSILON (1e-6)

sc_vec4_t *
sc_vec4_set(sc_vec4_t *vec, float x, float y, float z, float w)
{
    vec->x = x;
    vec->y = y;
    vec->z = z;
    vec->w = w;
    return vec;
}

sc_vec4_t *
sc_vec4_zero(sc_vec4_t *vec)
{
    vec->x = 0.0f;
    vec->y = 0.0f;
    vec->z = 0.0f;
    vec->w = 0.0f;
    return vec;
}

float
sc_vec4_length(const sc_vec4_t *vec)
{
    return sqrtf(sc_vec4_length2(vec));
}

float
sc_vec4_length2(const sc_vec4_t *vec)
{
    return vec->x * vec->x + vec->y * vec->y + vec->z * vec->z +
           vec->w * vec->w;
}

sc_vec4_t *
sc_vec4_normalize(sc_vec4_t *vec_out, const sc_vec4_t *vec)
{
    float factor, length = sc_vec4_length(vec);
    if (length == 0) /* assertions might cause troubles */
        return NULL;
    factor = 1.0f / length;
    vec_out->x = vec->x * factor;
    vec_out->y = vec->y * factor;
    vec_out->z = vec->z * factor;
    vec_out->w = vec->w * factor;
    return vec_out;
}

float
sc_vec4_dot(const sc_vec4_t *v1, const sc_vec4_t *v2)
{
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w;
}

sc_vec4_t *
sc_vec4_add(sc_vec4_t *vec_out, const sc_vec4_t *v1, const sc_vec4_t *v2)
{
    vec_out->x = v1->x + v2->x;
    vec_out->y = v1->y + v2->y;
    vec_out->z = v1->z + v2->z;
    vec_out->w = v1->w + v2->w;
    return vec_out;
}

sc_vec4_t *
sc_vec4_sub(sc_vec4_t *vec_out, const sc_vec4_t *v1, const sc_vec4_t *v2)
{
    vec_out->x = v1->x - v2->x;
    vec_out->y = v1->y - v2->y;
    vec_out->z = v1->z - v2->z;
    vec_out->w = v1->w - v2->w;
    return vec_out;
}

sc_vec4_t *
sc_vec4_mul(sc_vec4_t *vec_out, const sc_vec4_t *v, float factor)
{
    vec_out->x = v->x * factor;
    vec_out->y = v->y * factor;
    vec_out->z = v->z * factor;
    vec_out->w = v->w * factor;
    return vec_out;
}

sc_vec4_t *
sc_vec4_neg(sc_vec4_t *vec_out, const sc_vec4_t *vec)
{
    vec_out->x = -vec->x;
    vec_out->y = -vec->y;
    vec_out->z = -vec->z;
    vec_out->w = -vec->w;
    return vec_out;
}

sc_vec4_t *
sc_vec4_transform(sc_vec4_t *vec_out, const sc_vec4_t *vec,
                  const sc_mat4_t *mat)
{
    sc_vec4_t tmp;
    const float *m = mat->elms;
    tmp.x = vec->x * m[0] + vec->y * m[4] + vec->z * m[8] + vec->w * m[12];
    tmp.y = vec->x * m[1] + vec->y * m[5] + vec->z * m[9] + vec->w * m[13];
    tmp.z = vec->x * m[2] + vec->y * m[6] + vec->z * m[10] + vec->w * m[14];
    tmp.w = vec->x * m[3] + vec->y * m[7] + vec->z * m[11] + vec->w * m[15];
    *vec_out = tmp;
    return vec_out;
}

sc_vec3_t *
sc_vec4_transform_homogenous(sc_vec3_t *vec_out, const sc_vec4_t *vec,
                             const sc_mat4_t *mat)
{
    sc_vec4_t tmp;
    float inv;
    sc_vec4_transform(&tmp, vec, mat);
    if (tmp.w == 0.0f)
        return NULL;
    inv = 1.0f / tmp.w;
    vec_out->x = tmp.x * inv;
    vec_out->y = tmp.y * inv;
    vec_out->z = tmp.z * inv;
    return vec_out;
}

int
sc_vec4_equal(const sc_vec4_t *v1, const sc_vec4_t *v2)
{
    return (
        v1->x < v2->x + EPSILON &&
        v1->x > v2->x - EPSILON &&
        v1->y < v2->y + EPSILON &&
        v1->y > v2->y - EPSILON &&
        v1->z < v2->z + EPSILON &&
        v1->z > v2->z - EPSILON &&
        v1->w < v2->w + EPSILON &&
        v1->w > v2->w - EPSILON
    );
}

int
sc_vec4_nonzero(const sc_vec4_t *vec)
{
    return (
        vec->x >= EPSILON || vec->x <= -EPSILON ||
        vec->y >= EPSILON || vec->y <= -EPSILON ||
        vec->z >= EPSILON || vec->z <= -EPSILON ||
        vec->w >= EPSILON || vec->w <= -EPSILON
    );
}
