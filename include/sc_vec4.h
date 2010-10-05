/* Simple four dimensional vector */
#ifndef _INC_SC_VEC4_H_
#define _INC_SC_VEC4_H_

#include "sc_boot.h"

typedef struct {
    float x;
    float y;
    float z;
    float w;
} sc_vec4_t;

/* sets the vector values and returns it */
sc_vec4_t *sc_vec4_set(sc_vec4_t *vec, float x, float y, float z, float w);

/* sets the vector to zero and returns it */
sc_vec4_t *sc_vec4_zero(sc_vec4_t *vec);

/* returns the length of the vector */
float sc_vec4_length(const sc_vec4_t *vec);

/* returns the square length of the vector */
float sc_vec4_length2(const sc_vec4_t *vec);

/* normalizes a vector in place and returns it. */
sc_vec4_t *sc_vec4_normalize(sc_vec4_t *vec);

/* calculates the dot product of two vectors */
float sc_vec4_dot(const sc_vec4_t *v1, const sc_vec4_t *v2);

/* adds two vectors and stores in vec_out which is returend */
sc_vec4_t *sc_vec4_add(sc_vec4_t *vec_out, const sc_vec4_t *v1, const sc_vec4_t *v2);

/* subtracts two vectors and stores in vec_out which is returend */
sc_vec4_t *sc_vec4_sub(sc_vec4_t *vec_out, const sc_vec4_t *v1, const sc_vec4_t *v2);

/* multipies v by factor, stores result in vec_out which is returned */
sc_vec4_t *sc_vec4_mul(sc_vec4_t *vec_out, const sc_vec4_t *v, float factor);

/* negates a vector in place and returns it */
sc_vec4_t *sc_vec4_neg(sc_vec4_t *vec);

/* checks if two vectors are approximately the same */
int sc_vec4_equal(const sc_vec4_t *v1, const sc_vec4_t *v2);

/* checks if a vector is not zero in length or close to it */
int sc_vec4_nonzero(const sc_vec4_t *vec);

#endif
