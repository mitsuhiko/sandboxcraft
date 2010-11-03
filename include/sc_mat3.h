/* Simple three dimensional matrix */
#ifndef _INC_SC_MAT3_H_
#define _INC_SC_MAT3_H_

#include "sc_boot.h"

typedef struct {
    union {
        float elms[9];
        float m[3][3];
    };
} sc_mat3_t;

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
#define sc_mat3_debug(M) \
    fprintf(stderr, "[[%f, %f, %f],\n [%f, %f, %f],\n [%f, %f, %f]]\n", \
           (M)->elms[0], (M)->elms[1], (M)->elms[2], \
           (M)->elms[3], (M)->elms[4], (M)->elms[5], \
           (M)->elms[6], (M)->elms[7], (M)->elms[8])

#endif
