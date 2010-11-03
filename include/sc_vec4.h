/* Simple four dimensional vector */
#ifndef _INC_SC_VEC4_H_
#define _INC_SC_VEC4_H_

#include "sc_boot.h"

struct _sc_mat4;
struct _sc_vec3;
typedef struct _sc_vec4 {
    float x;
    float y;
    float z;
    float w;
} sc_vec4_t;

/* is the vector empty? */
#define sc_vec4_is_zero(vec) ((vec)->x == 0.0f && (vec)->y == 0.0f && \
                              (vec)->z == 0.0f && (vec)->z == 0.0f)

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

/* transforms the vector by the given matrix */
sc_vec4_t *sc_vec4_transform(sc_vec4_t *vec_out, const sc_vec4_t *vec,
                             const struct _sc_mat4 *mat);

/* transforms the vector by the given matrix.  This does not neglect the
   fourth parameter and returns a vec3. */
struct _sc_vec3 *sc_vec4_transform_homogenous(struct _sc_vec3 *vec_out,
                                              const sc_vec4_t *vec,
                                              const struct _sc_mat4 *mat);

/* checks if two vectors are approximately the same */
int sc_vec4_equal(const sc_vec4_t *v1, const sc_vec4_t *v2);

/* checks if a vector is not zero in length or close to it */
int sc_vec4_nonzero(const sc_vec4_t *vec);

/* prints the given vector to stderr plus a newline */
#define sc_vec4_debug(vec) \
    fprintf(stderr, "vec4(%f, %f, %f, %f)\n", (vec)->x, (vec)->y, (vec)->z, (vec)->w)

#endif
