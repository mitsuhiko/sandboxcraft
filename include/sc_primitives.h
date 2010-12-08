/* header that gives access to creating vbos of primitives. */
#ifndef _INC_SC_PRIMITIVES_H_
#define _INC_SC_PRIMITIVES_H_

#include "sc_vbo.h"

/* creates a new cube of a given size.  This also includes texture
   coordinates where the ring around the cube has standing texture
   coordiantes, the top is flipped and the bottom as well.

   This is mostly a shortcut for the following six functions.

   The vbo is not yet finalized. */
sc_vbo_t *sc_new_cube(float size);

/* individual cube creation */
void sc_cube_add_near_plane(sc_vbo_t *vbo, float size, float off_x,
                            float off_y, float off_z);
void sc_cube_add_far_plane(sc_vbo_t *vbo, float size, float off_x,
                           float off_y, float off_z);
void sc_cube_add_top_plane(sc_vbo_t *vbo, float size, float off_x,
                           float off_y, float off_z);
void sc_cube_add_bottom_plane(sc_vbo_t *vbo, float size, float off_x,
                              float off_y, float off_z);
void sc_cube_add_left_plane(sc_vbo_t *vbo, float size, float off_x,
                            float off_y, float off_z);
void sc_cube_add_right_plane(sc_vbo_t *vbo, float size, float off_x,
                             float off_y, float off_z);

#endif
