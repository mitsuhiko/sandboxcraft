/* implements a basic quaternion data type */
#ifndef _INC_SC_QUATERNION_H_
#define _INC_SC_QUATERNION_H_

#include "sc_boot.h"
#include "sc_vec3.h"

typedef struct {
    float w;
    sc_vec3_t vec;
} sc_quaternion_t;

/* sets a quaterion values and returns it */
sc_quaternion_t *sc_quaternion_set(sc_quaternion_t *quat, float w, float x,
                                   float y, float z);

/* zeroes out a quaternion and returns it */
sc_quaternion_t *sc_quaternion_zero(sc_quaternion_t *quat);

/* negates the given quaternion in place and returns it. */
sc_quaternion_t *sc_quaternion_neg(sc_quaternion_t *quat);

/* checks if two quaternions are nearly equal. */
int sc_quaternion_equal(const sc_quaternion_t *quat1,
                        const sc_quaternion_t *quat2);

#endif
