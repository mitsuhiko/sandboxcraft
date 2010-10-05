#include "sc_camera.h"

sc_camera_t *
sc_new_camera(void)
{
    sc_camera_t *rv = sc_xalloc(sc_camera_t);
    sc_vec3_zero(&rv->position);
    sc_quaternion_set_identity(&rv->orientation);
    return rv;
}

void
sc_free_camera(sc_camera_t *cam)
{
    if (!cam)
        return;
    sc_free(cam);
}
