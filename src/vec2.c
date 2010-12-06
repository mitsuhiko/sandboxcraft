#include "sc_vec2.h"
#include "sc_math.h"

#define EPSILON (1e-6)

sc_vec2_t *
sc_vec2_set(sc_vec2_t *vec, float x, float y)
{
    vec->x = x;
    vec->y = y;
    return vec;
}

sc_vec2_t *
sc_vec2_zero(sc_vec2_t *vec)
{
    vec->x = 0.0f;
    vec->y = 0.0f;
    return vec;
}

float
sc_vec2_length(const sc_vec2_t *vec)
{
    return sqrtf(sc_vec2_length2(vec));
}

float
sc_vec2_length2(const sc_vec2_t *vec)
{
    return vec->x * vec->x + vec->y * vec->y;
}

sc_vec2_t *
sc_vec2_normalize(sc_vec2_t *vec_out, const sc_vec2_t *vec)
{
    float factor, length = sc_vec2_length(vec);
    if (length == 0) /* assertions might cause troubles */
        return NULL;
    factor = 1.0f / length;
    vec_out->x = vec->x * factor;
    vec_out->y = vec->y * factor;
    return vec_out;
}

float
sc_vec2_cross(const sc_vec2_t *v1, const sc_vec2_t *v2)
{
    return v1->x * v2->y - v1->y * v2->x;
}

float
sc_vec2_dot(const sc_vec2_t *v1, const sc_vec2_t *v2)
{
    return v1->x * v2->x + v1->y * v2->y;
}

sc_vec2_t *
sc_vec2_add(sc_vec2_t *vec_out, const sc_vec2_t *v1, const sc_vec2_t *v2)
{
    vec_out->x = v1->x + v2->x;
    vec_out->y = v1->y + v2->y;
    return vec_out;
}

sc_vec2_t *
sc_vec2_sub(sc_vec2_t *vec_out, const sc_vec2_t *v1, const sc_vec2_t *v2)
{
    vec_out->x = v1->x - v2->x;
    vec_out->y = v1->y - v2->y;
    return vec_out;
}

sc_vec2_t *
sc_vec2_mul(sc_vec2_t *vec_out, const sc_vec2_t *v, float factor)
{
    vec_out->x = v->x * factor;
    vec_out->y = v->y * factor;
    return vec_out;
}

sc_vec2_t *
sc_vec2_neg(sc_vec2_t *vec_out, const sc_vec2_t *vec)
{
    vec_out->x = -vec->x;
    vec_out->y = -vec->y;
    return vec_out;
}

int
sc_vec2_equal(const sc_vec2_t *v1, const sc_vec2_t *v2)
{
    return (
        v1->x < v2->x + EPSILON &&
        v1->x > v2->x - EPSILON &&
        v1->y < v2->y + EPSILON &&
        v1->y > v2->y - EPSILON
    );
}

int
sc_vec2_nonzero(const sc_vec2_t *vec)
{
    return (
        vec->x >= EPSILON || vec->x <= -EPSILON ||
        vec->y >= EPSILON || vec->y <= -EPSILON
    );
}
