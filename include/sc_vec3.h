/* Simple three dimensional vector */
#ifndef _INC_SC_VEC3_H_
#define _INC_SC_VEC3_H_

#include "sc_boot.h"

struct _sc_mat4;
typedef struct _sc_vec3 {
    float x;
    float y;
    float z;
} sc_vec3_t;

/* is the vector empty? */
static sc_inline int
sc_vec3_is_zero(const sc_vec3_t *vec)
{
    return (vec->x == 0.0f && vec->y == 0.0f && vec->z == 0.0f);
}

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

/* negates a vector in place and returns it */
sc_vec3_t *sc_vec3_neg(sc_vec3_t *vec);

/* transforms the vector by the given matrix */
sc_vec3_t *sc_vec3_transform(sc_vec3_t *vec_out, const sc_vec3_t *vec,
                             const struct _sc_mat4 *mat);

/* transforms the vector by the given matrix.  This performs a homogenous
   transformation where the vector is converted into a vec4.  This is a
   shortcut for sc_vec4_transform_homogenous. */
sc_vec3_t *sc_vec3_transform_homogenous(sc_vec3_t *vec_out, const sc_vec3_t *vec,
                                        const struct _sc_mat4 *mat);

/* checks if two vectors are approximately the same */
int sc_vec3_equal(const sc_vec3_t *v1, const sc_vec3_t *v2);

/* checks if a vector is not zero in length or close to it */
int sc_vec3_nonzero(const sc_vec3_t *vec);

/* prints the given vector to stderr plus a newline */
static sc_inline void
sc_vec3_debug(const sc_vec3_t *vec)
{
    fprintf(stderr, "vec3(%f, %f, %f)\n", vec->x, vec->y, vec->z);
}

#endif
