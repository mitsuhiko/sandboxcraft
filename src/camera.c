#include "sc_camera.h"
#include "sc_engine.h"
#include "sc_math.h"
#include "sc_mat4.h"

#define NEAR_PLANE 1.0f
#define FAR_PLANE 1000.0f

sc_camera_t *
sc_new_camera(void)
{
    sc_camera_t *rv = sc_xalloc(sc_camera_t);
    sc_vec3_zero(&rv->position);
    sc_vec3_set(&rv->forward, 0.0f, 0.0f, -1.0f);
    sc_vec3_set(&rv->up, 0.0f, 1.0f, 0.0f);
    rv->fov = sc_config.fov;
    return rv;
}

void
sc_free_camera(sc_camera_t *cam)
{
    if (!cam)
        return;
    sc_free(cam);
}

void
sc_camera_set_position(sc_camera_t *cam, float x, float y, float z)
{
    sc_vec3_set(&cam->position, x, y, z);
}

void
sc_camera_look_at(sc_camera_t *cam, float x, float y, float z)
{
    sc_vec3_t vec;
    sc_vec3_set(&vec, x, y, z);
    sc_camera_look_at_vector(cam, &vec);
}

void
sc_camera_look_at_vector(sc_camera_t *cam, const sc_vec3_t *vec)
{
    sc_vec3_sub(&cam->forward, vec, &cam->position);
    sc_vec3_normalize(&cam->forward, &cam->forward);
}

static void
rotate_screen_x(sc_camera_t *cam, float angle)
{
    sc_mat4_t rotmat;
    sc_mat4_from_axis_rotation_vector(&rotmat, -angle, &cam->up);
    sc_vec3_transform(&cam->forward, &cam->forward, &rotmat);
}

static void
rotate_screen_y(sc_camera_t *cam, float angle)
{
    sc_vec3_t cross;
    sc_mat4_t rotmat;
    sc_vec3_cross(&cross, &cam->up, &cam->forward);
    sc_mat4_from_axis_rotation_vector(&rotmat, angle, &cross);
    sc_vec3_transform(&cam->forward, &cam->forward, &rotmat);
}

void
sc_camera_rotate_screen(sc_camera_t *cam, float relx, float rely)
{
    if (relx != 0.0f)
        rotate_screen_x(cam, relx);
    if (rely != 0.0f)
        rotate_screen_y(cam, rely);
}

void
sc_camera_move_forward(sc_camera_t *cam, float delta)
{
    sc_vec3_t vec;
    sc_vec3_mul(&vec, &cam->forward, delta);
    sc_vec3_add(&cam->position, &cam->position, &vec);
}

void
sc_camera_move_backward(sc_camera_t *cam, float delta)
{
    sc_vec3_t vec;
    sc_vec3_mul(&vec, &cam->forward, delta);
    sc_vec3_sub(&cam->position, &cam->position, &vec);
}

void
sc_camera_strafe_left(sc_camera_t *cam, float delta)
{
    sc_vec3_t vec, cross;
    sc_vec3_cross(&cross, &cam->up, &cam->forward);
    sc_vec3_mul(&vec, &cross, delta);
    sc_vec3_add(&cam->position, &cam->position, &vec);
}

void
sc_camera_strafe_right(sc_camera_t *cam, float delta)
{
    sc_vec3_t vec, cross;
    sc_vec3_cross(&cross, &cam->up, &cam->forward);
    sc_vec3_mul(&vec, &cross, delta);
    sc_vec3_sub(&cam->position, &cam->position, &vec);
}

void
sc_camera_apply(const sc_camera_t *cam)
{
    sc_mat4_t projection, modelview, identity;
    sc_mat4_set_perspective(&projection, cam->fov, sc_engine_get_aspect(),
                            NEAR_PLANE, FAR_PLANE);
    sc_engine_set_projection_matrix(&projection);
    sc_mat4_look_at(&modelview, &cam->position, &cam->forward, &cam->up);
    sc_engine_set_view_matrix(&modelview);
    sc_mat4_set_identity(&identity);
    sc_engine_set_model_matrix(&identity);
}
