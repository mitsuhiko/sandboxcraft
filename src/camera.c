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
    rv->fov = 55.0f;
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
    cam->position.x = x;
    cam->position.y = y;
    cam->position.z = z;
}

void
sc_camera_look_at(sc_camera_t *cam, float x, float y, float z)
{
    sc_vec3_t vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    sc_camera_look_at_vector(cam, &vec);
}

void
sc_camera_look_at_vector(sc_camera_t *cam, const sc_vec3_t *vec)
{
    sc_vec3_t diff, cross;
    sc_vec3_cross(&cross, &cam->forward, &cam->up);
    sc_vec3_sub(&diff, &cam->position, vec);
    sc_vec3_normalize(&diff);
    cam->forward = diff;
    sc_vec3_cross(&cam->up, &cross, &cam->forward);
}

static void
rotate_screen_x(sc_camera_t *cam, float angle)
{
    sc_mat4_t rotmat;
    sc_mat4_from_axis_rotation(&rotmat, -angle, &cam->up);
    sc_vec3_transform(&cam->forward, &cam->forward, &rotmat);
}

static void
rotate_screen_y(sc_camera_t *cam, float angle)
{
    sc_vec3_t cross;
    sc_mat4_t rotmat;
    sc_vec3_cross(&cross, &cam->up, &cam->forward);
    sc_mat4_from_axis_rotation(&rotmat, angle, &cross);
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(cam->fov, sc_engine_get_aspect(), NEAR_PLANE, FAR_PLANE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cam->position.x, cam->position.y, cam->position.z,
                cam->position.x + cam->forward.x,
                cam->position.y + cam->forward.y,
                cam->position.z + cam->forward.z,
              cam->up.x, cam->up.y, cam->up.z);
}
