/* Simple four dimensional matrix */
#ifndef _INC_SC_MAT4_H_
#define _INC_SC_MAT4_H_

#include "sc_boot.h"
#include "sc_vec3.h"

struct _sc_quaternion;
typedef struct _sc_mat4 {
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

#endif
