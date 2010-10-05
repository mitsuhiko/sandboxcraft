#include "sc_quaternion.h"

#define EPSILON (1e-6)

sc_quaternion_t *
sc_quaternion_set(sc_quaternion_t *quat, float w, float x, float y, float z)
{
    quat->w = w;
    sc_vec3_set(&quat->vec, x, y, z);
    return quat;
}

sc_quaternion_t *
sc_quaternion_zero(sc_quaternion_t *quat)
{
    quat->w = 0.0f;
    sc_vec3_zero(&quat->vec);
    return quat;
}

sc_quaternion_t *
sc_quaternion_neg(sc_quaternion_t *quat)
{
    quat->w = -quat->w;
    sc_vec3_neg(&quat->vec);
    return quat;
}

int
sc_quaternion_equal(const sc_quaternion_t *quat1,
                    const sc_quaternion_t *quat2)
{
    return (
        quat1->w < quat2->w + EPSILON &&
        quat1->w > quat2->w - EPSILON &&
        sc_vec3_equal(&quat1->vec, &quat2->vec)
    );
}
