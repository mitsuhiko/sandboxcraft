/* Simple four dimensional matrix */
#ifndef _INC_SC_MAT4_H_
#define _INC_SC_MAT4_H_

#include "sc_boot.h"

typedef struct {
    float elms[16];
} sc_mat4_t;

/* helper macro to address a piece of the matrix */
#define sc_mat4(mat, row, column) ((mat)->elms[(column) + (row) * 4])

/* sets the values of the matrix */
sc_mat4_t *sc_mat4_set(sc_mat4_t *mat, float a, float b, float c, float d,
                       float e, float f, float g, float h, float i,
                       float j, float k, float l, float m, float n,
                       float o, float p);

/* sets the matrix to the identity matrix */
sc_mat4_t *sc_mat4_set_identity(sc_mat4_t *mat);

#endif
