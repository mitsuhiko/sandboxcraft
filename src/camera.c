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
    sc_vec3_set(&rv->forward, 0.0f, 1.0f, 0.0f);
    sc_vec3_set(&rv->up, 0.0f, 0.0f, -1.0f);
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

static void
rotate_screen_x(sc_camera_t *cam, float angle)
{
    sc_vec3_t tmpvec;
    sc_mat4_t rotmat;
    float *m = rotmat.elms;
    sc_mat4_from_axis_rotation(&rotmat, -angle, &cam->up);

    tmpvec.x = m[0] * cam->forward.x + m[4] * cam->forward.y + m[8] * cam->forward.z;
    tmpvec.y = m[1] * cam->forward.x + m[5] * cam->forward.y + m[9] * cam->forward.z;
    tmpvec.z = m[2] * cam->forward.x + m[6] * cam->forward.y + m[10] * cam->forward.z;
    cam->forward = tmpvec;
}

static void
rotate_screen_y(sc_camera_t *cam, float angle)
{
    sc_vec3_t cross, tmpvec;
    sc_mat4_t rotmat;
    float *m = rotmat.elms;

    sc_vec3_cross(&cross, &cam->up, &cam->forward);
    sc_mat4_from_axis_rotation(&rotmat, angle, &cross);

    tmpvec.x = m[0] * cam->forward.x + m[4] * cam->forward.y + m[8] * cam->forward.z;
    tmpvec.y = m[1] * cam->forward.x + m[5] * cam->forward.y + m[9] * cam->forward.z;
    tmpvec.z = m[2] * cam->forward.x + m[6] * cam->forward.y + m[10] * cam->forward.z;
    cam->forward = tmpvec;

    tmpvec.x = m[0] * cam->up.x + m[4] * cam->up.y + m[8] * cam->up.z;
    tmpvec.y = m[1] * cam->up.x + m[5] * cam->up.y + m[9] * cam->up.z;
    tmpvec.z = m[2] * cam->up.x + m[6] * cam->up.y + m[10] * cam->up.z;
    cam->up = tmpvec;
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
