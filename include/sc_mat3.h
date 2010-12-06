/* Simple three dimensional matrix */
#ifndef _INC_SC_MAT3_H_
#define _INC_SC_MAT3_H_

#include "sc_boot.h"

typedef struct _sc_mat3 {
    union {
        float elms[9];
        float m[3][3];
    };
} sc_mat3_t;

/* return pointer to the first value in the matrix */
#define sc_mat3_ptr(M) (&((M)->elms[0]))

/* sets the values of the matrix */
sc_mat3_t *sc_mat3_set(sc_mat3_t *mat, float a, float b, float c, float d,
                       float e, float f, float g, float h, float i);

/* sets the matrix to the identity matrix */
sc_mat3_t *sc_mat3_set_identity(sc_mat3_t *mat);

/* returns the determinant of the matrix */
float sc_mat3_determinant(const sc_mat3_t *mat);

/* returns the adjugate matrix */
sc_mat3_t * sc_mat3_adjugate(sc_mat3_t *mat_out, const sc_mat3_t *mat);

/* calculate the inverse if possible.  NULL is returned if calculation
   is not possible. */
sc_mat3_t *sc_mat3_inverse(sc_mat3_t *mat_out, const sc_mat3_t *mat);

/* multiplies a matrix with another matrix */
sc_mat3_t *sc_mat3_mul(sc_mat3_t *mat_out, const sc_mat3_t *mat1,
                       const sc_mat3_t *mat2);

/* multiply the matrix by a factor */
sc_mat3_t *sc_mat3_scalar_mul(sc_mat3_t *mat_out, const sc_mat3_t *mat, float factor);

/* transposes the matrix */
sc_mat3_t *sc_mat3_transpose(sc_mat3_t *mat_out, const sc_mat3_t *mat);

/* prints matrix to stderr */
static sc_inline void
sc_mat3_debug(const sc_mat3_t *mat)
{
    fprintf(stderr, "[[%f, %f, %f],\n [%f, %f, %f],\n [%f, %f, %f]]\n",
           mat->elms[0], mat->elms[1], mat->elms[2],
           mat->elms[3], mat->elms[4], mat->elms[5],
           mat->elms[6], mat->elms[7], mat->elms[8]);
}

#endif
