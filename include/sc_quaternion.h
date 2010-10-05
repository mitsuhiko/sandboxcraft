/* implements a basic quaternion data type */
#ifndef _INC_SC_QUATERNION_H_
#define _INC_SC_QUATERNION_H_

#include "sc_boot.h"
#include "sc_vec3.h"

struct _sc_mat4;
typedef struct _sc_quaternion {
    float w;
    sc_vec3_t vec;
} sc_quaternion_t;

/* sets a quaterion values and returns it */
sc_quaternion_t *sc_quaternion_set(sc_quaternion_t *quat, float w, float x,
                                   float y, float z);

/* sets the quaternion to identity */
sc_quaternion_t *sc_quaternion_set_identity(sc_quaternion_t *quat);

/* zeroes out a quaternion and returns it */
sc_quaternion_t *sc_quaternion_zero(sc_quaternion_t *quat);

/* negates the given quaternion in place and returns it. */
sc_quaternion_t *sc_quaternion_neg(sc_quaternion_t *quat);

/* checks if two quaternions are nearly equal. */
int sc_quaternion_equal(const sc_quaternion_t *quat1,
                        const sc_quaternion_t *quat2);

/* converts a quaternion into a rotation matrix */
struct _sc_mat4 *sc_quaternion_to_mat4(struct _sc_mat4 *mat_out,
                                       const sc_quaternion_t *quat);

#endif
