/* Simple two dimensional vector */
#ifndef _INC_SC_VEC2_H_
#define _INC_SC_VEC2_H_

#include "sc_boot.h"

typedef struct {
    float x;
    float y;
} sc_vec2_t;

/* is the vector empty? */
static sc_inline int
sc_vec2_is_zero(const sc_vec2_t *vec)
{
    return (vec->x == 0.0f && vec->y == 0.0f);
}

/* sets the vector values and returns it */
sc_vec2_t *sc_vec2_set(sc_vec2_t *vec, float x, float y);

/* sets the vector to zero and returns it */
sc_vec2_t *sc_vec2_zero(sc_vec2_t *vec);

/* returns the length of the vector */
float sc_vec2_length(const sc_vec2_t *vec);

/* returns the square length of the vector */
float sc_vec2_length2(const sc_vec2_t *vec);

/* normalizes a vector in place and returns it. */
sc_vec2_t *sc_vec2_normalize(sc_vec2_t *vec);

/* calculates the cross product of two vectors */
float sc_vec2_cross(const sc_vec2_t *v1, const sc_vec2_t *v2);

/* calculates the dot product of two vectors */
float sc_vec2_dot(const sc_vec2_t *v1, const sc_vec2_t *v2);

/* adds two vectors and stores in vec_out which is returend */
sc_vec2_t *sc_vec2_add(sc_vec2_t *vec_out, const sc_vec2_t *v1, const sc_vec2_t *v2);

/* subtracts two vectors and stores in vec_out which is returend */
sc_vec2_t *sc_vec2_sub(sc_vec2_t *vec_out, const sc_vec2_t *v1, const sc_vec2_t *v2);

/* multipies v by factor, stores result in vec_out which is returned */
sc_vec2_t *sc_vec2_mul(sc_vec2_t *vec_out, const sc_vec2_t *v, float factor);

/* negates a vector in place and returns it */
sc_vec2_t *sc_vec2_neg(sc_vec2_t *vec);

/* checks if two vectors are approximately the same */
int sc_vec2_equal(const sc_vec2_t *v1, const sc_vec2_t *v2);

/* checks if a vector is not zero in length or close to it */
int sc_vec2_nonzero(const sc_vec2_t *vec);

/* prints the given vector to stderr plus a newline */
static sc_inline void
sc_vec2_debug(const sc_vec2_t *vec)
{
    fprintf(stderr, "vec2(%f, %f)\n", vec->x, vec->y);
}

#endif
