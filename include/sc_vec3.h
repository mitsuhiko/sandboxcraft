/* Simple three dimenstional vector */
#ifndef _INC_SC_VEC3_H_
#define _INC_SC_VEC3_H_

#include "sc_boot.h"

typedef struct {
    float x;
    float y;
    float z;
} sc_vec3_t;

/* sets the vector values and returns it */
sc_vec3_t *sc_vec3_set(sc_vec3_t *vec, float x, float y, float z);

/* sets the vector to zero and returns it */
sc_vec3_t *sc_vec3_zero(sc_vec3_t *vec);

/* returns the length of the vector */
float sc_vec3_length(const sc_vec3_t *vec);

/* returns the square length of the vector */
float sc_vec3_length2(const sc_vec3_t *vec);

/* normalizes a vector in place and returns it. */
sc_vec3_t *sc_vec3_normalize(sc_vec3_t *vec);

/* calculates the cross product of two vectors, stores it in vec_out
   and returns a pointer to it. */
sc_vec3_t *sc_vec3_cross(sc_vec3_t *vec_out, const sc_vec3_t *v1, const sc_vec3_t *v2);

/* calculates the dot product of two vectors */
float sc_vec3_dot(const sc_vec3_t *v1, const sc_vec3_t *v2);

/* adds two vectors and stores in vec_out which is returend */
sc_vec3_t *sc_vec3_add(sc_vec3_t *vec_out, const sc_vec3_t *v1, const sc_vec3_t *v2);

/* subtracts two vectors and stores in vec_out which is returend */
sc_vec3_t *sc_vec3_sub(sc_vec3_t *vec_out, const sc_vec3_t *v1, const sc_vec3_t *v2);

/* multipies v by factor, stores result in vec_out which is returned */
sc_vec3_t *sc_vec3_mul(sc_vec3_t *vec_out, const sc_vec3_t *v, float factor);

/* checks if two vectors are approximately the same */
int sc_vec3_equal(const sc_vec3_t *v1, const sc_vec3_t *v2);

/* checks if a vector is not zero in length or close to it */
int sc_vec3_nonzero(const sc_vec3_t *vec);

#endif
