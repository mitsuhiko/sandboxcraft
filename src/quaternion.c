#include "sc_quaternion.h"
#include "sc_mat4.h"

#define EPSILON (1e-6)

sc_quaternion_t *
sc_quaternion_set(sc_quaternion_t *quat, float w, float x, float y, float z)
{
    quat->w = w;
    sc_vec3_set(&quat->vec, x, y, z);
    return quat;
}

sc_quaternion_t *
sc_quaternion_set_identity(sc_quaternion_t *quat)
{
    quat->w = 1.0f;
    sc_vec3_set(&quat->vec, 0.0f, 0.0f, 0.0f);
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

sc_mat4_t *
sc_quaternion_to_mat4(sc_mat4_t *mat_out, const sc_quaternion_t *quat)
{
    mat_out->elms[0]  = 1.0f - 2.0f * (quat->vec.y * quat->vec.y + quat->vec.z * quat->vec.z);
    mat_out->elms[4]  = 2.0f * (quat->vec.x * quat->vec.y + quat->vec.z * quat->w);
    mat_out->elms[8]  = 2.0f * (quat->vec.x * quat->vec.z - quat->vec.y * quat->w);
    mat_out->elms[12] = 0.0f;

    mat_out->elms[1]  = 2.0f * (quat->vec.x * quat->vec.y - quat->vec.z * quat->w );
    mat_out->elms[5]  = 1.0f - 2.0f * (quat->vec.x * quat->vec.x + quat->vec.z * quat->vec.z);
    mat_out->elms[9]  = 2.0f * (quat->vec.z * quat->vec.y + quat->vec.x * quat->w );
    mat_out->elms[13] = 0.0f;

    mat_out->elms[2]  = 2.0f * (quat->vec.x * quat->vec.z + quat->vec.y * quat->w);
    mat_out->elms[6]  = 2.0f * (quat->vec.y * quat->vec.z - quat->vec.x * quat->w);
    mat_out->elms[10] = 1.0f - 2.0f * (quat->vec.x * quat->vec.x + quat->vec.y * quat->vec.y);
    mat_out->elms[14] = 0.0f;

    mat_out->elms[3]  = 0;
    mat_out->elms[7]  = 0;
    mat_out->elms[11] = 0;
    mat_out->elms[15] = 1.0f;

    return mat_out;
}
