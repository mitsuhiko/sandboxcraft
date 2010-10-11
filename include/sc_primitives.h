/* header that gives access to creating vbos of primitives. */
#ifndef _INC_SC_PRIMITIVES_H_
#define _INC_SC_PRIMITIVES_H_

#include "sc_vbo.h"

/* creates a new cube of a given size.  This also includes texture
   coordinates where the ring around the cube has standing texture
   coordiantes, the top is flipped and the bottom as well.

   This is nice for debugging purposes but it does not really give
   one the ability to modify the cube because when this function
   exists the vbo is already finalized. */
sc_vbo_t *sc_new_cube(float size);

#endif
