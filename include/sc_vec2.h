/* Simple two dimensional vector */
#ifndef _INC_SC_VEC2_H_
#define _INC_SC_VEC2_H_

#include "sc_boot.h"

typedef struct {
    float x;
    float y;
} sc_vec2_t;

/* is the vector empty? */
#define sc_vec2_is_zero(vec) ((vec)->x == 0.0f && (vec)->y == 0.0f)

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

/* other comparisions */
int sc_vec2_lt(const sc_vec2_t *v1, const sc_vec2_t *v2);
int sc_vec2_le(const sc_vec2_t *v1, const sc_vec2_t *v2);
int sc_vec2_gt(const sc_vec2_t *v1, const sc_vec2_t *v2);
int sc_vec2_ge(const sc_vec2_t *v1, const sc_vec2_t *v2);

/* checks if a vector is not zero in length or close to it */
int sc_vec2_nonzero(const sc_vec2_t *vec);

#endif
