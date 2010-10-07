/* helper struct and functions for cam management */
#ifndef _INC_SC_CAMERA_H_
#define _INC_SC_CAMERA_H_

#include "sc_boot.h"
#include "sc_vec3.h"

typedef struct {
    sc_vec3_t position;
    sc_vec3_t forward;
    sc_vec3_t up;
    float fov;
} sc_camera_t;

sc_camera_t *sc_new_camera(void);
void sc_free_camera(sc_camera_t *cam);

void sc_camera_set_position(sc_camera_t *cam, float x, float y, float z);
void sc_camera_rotate_screen(sc_camera_t *cam, float relx, float rely);

/* sends the transformation matrix to the graphics device */
void sc_camera_apply(const sc_camera_t *cam);

#endif
