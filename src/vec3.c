#include <math.h>

#include "sc_vec3.h"

#define EPSILON (1e-6)

sc_vec3_t *
sc_vec3_set(sc_vec3_t *vec, float x, float y, float z)
{
    vec->x = x;
    vec->y = y;
    vec->z = z;
    return vec;
}

sc_vec3_t *
sc_vec3_zero(sc_vec3_t *vec)
{
    vec->x = 0.0f;
    vec->y = 0.0f;
    vec->z = 0.0f;
    return vec;
}

float
sc_vec3_length(const sc_vec3_t *vec)
{
    return sqrtf(sc_vec3_length2(vec));
}

float
sc_vec3_length2(const sc_vec3_t *vec)
{
    return vec->x * vec->x + vec->y * vec->y + vec->z * vec->z;
}

sc_vec3_t *
sc_vec3_normalize(sc_vec3_t *vec)
{
    float factor = 1.0f / sc_vec3_length(vec);
    vec->x = vec->x * factor;
    vec->y = vec->y * factor;
    vec->z = vec->z * factor;
    return vec;
}

sc_vec3_t *
sc_vec3_cross(sc_vec3_t *vec_out, const sc_vec3_t *v1, const sc_vec3_t *v2)
{
    vec_out->x = (v1->y * v2->z) - (v1->z * v2->y);
    vec_out->y = (v1->z * v2->z) - (v1->x * v2->z);
    vec_out->z = (v1->x * v2->y) - (v1->y * v2->x);
    return vec_out;
}

float
sc_vec3_dot(const sc_vec3_t *v1, const sc_vec3_t *v2)
{
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

sc_vec3_t *
sc_vec3_add(sc_vec3_t *vec_out, const sc_vec3_t *v1, const sc_vec3_t *v2)
{
    vec_out->x = v1->x + v2->x;
    vec_out->y = v1->y + v2->y;
    vec_out->z = v1->z + v2->z;
    return vec_out;
}

sc_vec3_t *
sc_vec3_sub(sc_vec3_t *vec_out, const sc_vec3_t *v1, const sc_vec3_t *v2)
{
    vec_out->x = v1->x - v2->x;
    vec_out->y = v1->y - v2->y;
    vec_out->z = v1->z - v2->z;
    return vec_out;
}

sc_vec3_t *
sc_vec3_mul(sc_vec3_t *vec_out, const sc_vec3_t *v, float factor)
{
    vec_out->x = v->x * factor;
    vec_out->y = v->y * factor;
    vec_out->z = v->z * factor;
    return vec_out;
}

sc_vec3_t *
sc_vec3_neg(sc_vec3_t *vec)
{
    vec->x = -vec->x;
    vec->y = -vec->y;
    vec->z = -vec->z;
    return vec;
}

int
sc_vec3_equal(const sc_vec3_t *v1, const sc_vec3_t *v2)
{
    return (
        v1->x < v2->x + EPSILON &&
        v1->x > v2->x - EPSILON &&
        v1->y < v2->y + EPSILON &&
        v1->y > v2->y - EPSILON &&
        v1->z < v2->z + EPSILON &&
        v1->z > v2->z - EPSILON
    );
}

int
sc_vec3_nonzero(const sc_vec3_t *vec)
{
    return (
        vec->x >= EPSILON || vec->x <= -EPSILON ||
        vec->y >= EPSILON || vec->y <= -EPSILON ||
        vec->z >= EPSILON || vec->z <= -EPSILON
    );
}
