/* Simple three dimensional matrix */
#ifndef _INC_SC_MAT3_H_
#define _INC_SC_MAT3_H_

#include "sc_boot.h"

typedef struct {
    float elms[9];
} sc_mat3_t;

/* helper macro to address a piece of the matrix */
#define sc_mat3(mat, row, column) ((mat)->elms[(column) + (row) * 3])

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

#endif
