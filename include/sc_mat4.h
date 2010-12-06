/* Simple four dimensional matrix */
#ifndef _INC_SC_MAT4_H_
#define _INC_SC_MAT4_H_

#include "sc_boot.h"
#include "sc_vec3.h"

typedef struct _sc_mat4 {
    union {
        float m[4][4];      /* m[row][column] */
        float elms[16];     /* elms[index] */
    };
} sc_mat4_t;

/* return pointer to the first value in the matrix */
#define sc_mat4_ptr(M) (&((M)->elms[0]))

/* sets the values of the matrix */
sc_mat4_t *sc_mat4_set(sc_mat4_t *mat, float a, float b, float c, float d,
                       float e, float f, float g, float h, float i,
                       float j, float k, float l, float m, float n,
                       float o, float p);

/* sets the matrix to the identity matrix */
sc_mat4_t *sc_mat4_set_identity(sc_mat4_t *mat);

/* sets the matrix to a rotation matrix */
sc_mat4_t *sc_mat4_from_axis_rotation(sc_mat4_t *mat, float angle,
                                      const sc_vec3_t *axis);

/* returns the determinant of the matrix */
float sc_mat4_determinant(const sc_mat4_t *mat);

/* returns the adjugate matrix */
sc_mat4_t * sc_mat4_adjugate(sc_mat4_t *mat_out, const sc_mat4_t *mat);

/* calculate the inverse if possible.  NULL is returned if calculation
   is not possible. */
sc_mat4_t *sc_mat4_inverse(sc_mat4_t *mat_out, const sc_mat4_t *mat);

/* multiplies two matrices */
sc_mat4_t *sc_mat4_mul(sc_mat4_t *mat_out, const sc_mat4_t *mat1,
                       const sc_mat4_t *mat2);

/* multiplies a matrix with a scalar factor */
sc_mat4_t *sc_mat4_scalar_mul(sc_mat4_t *mat_out, const sc_mat4_t *mat,
                              float factor);

/* transposes a matrix */
sc_mat4_t *sc_mat4_transpose(sc_mat4_t *mat_out, const sc_mat4_t *mat);

/* checks if a matrix equals another matrix */
int sc_mat4_equal(const sc_mat4_t *mat1, const sc_mat4_t *mat2);

/* prints matrix to stderr */
static sc_inline void
sc_mat4_debug(const sc_mat4_t *mat)
{
    fprintf(stderr, "[[%f, %f, %f, %f],\n [%f, %f, %f, %f],\n"
           " [%f, %f, %f, %f],\n [%f, %f, %f, %f]]\n",
           mat->elms[0], mat->elms[1], mat->elms[2], mat->elms[3],
           mat->elms[4], mat->elms[5], mat->elms[6], mat->elms[7],
           mat->elms[8], mat->elms[9], mat->elms[10], mat->elms[11],
           mat->elms[12], mat->elms[13], mat->elms[14], mat->elms[15]);
}

#endif
