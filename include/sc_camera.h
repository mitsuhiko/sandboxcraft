/* helper struct and functions for cam management */
#ifndef _INC_SC_CAMERA_H_
#define _INC_SC_CAMERA_H_

#include "sc_boot.h"
#include "sc_vec3.h"

typedef struct {
    sc_vec3_t position;
} sc_camera_t;

sc_camera_t *sc_new_camera(void);
void sc_free_camera(sc_camera_t *cam);

#endif
