/* Simple three dimenstional vector */
#ifndef _INC_SC_MAT3_H_
#define _INC_SC_MAT3_H_

#include "sc_boot.h"

typedef struct {
    float elms[9];
} sc_mat3_t;

/* helper macro to address a piece of the matrix */
#define sc_mat3(mat, row, column) ((mat)->elms[(column) + (row) * 3])

/* macro that returns a pointer to a matrix */
#define sc_mat3_ptr(mat) (&(mat)->elms)

/* sets the values of the matrix */
sc_mat3_t *sc_mat3_set(sc_mat3_t *mat, float a, float b, float c, float d,
                       float e, float f, float g, float h, float i);

/* sets the matrix to the identity matrix */
sc_mat3_t *sc_mat3_set_identity(sc_mat3_t *mat);

#endif
