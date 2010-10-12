/* header that gives access to creating vbos of primitives. */
#ifndef _INC_SC_PRIMITIVES_H_
#define _INC_SC_PRIMITIVES_H_

#include "sc_vbo.h"

/* creates a new cube of a given size.  This also includes texture
   coordinates where the ring around the cube has standing texture
   coordiantes, the top is flipped and the bottom as well.

   The vbo is not yet finalized. */
sc_vbo_t *sc_new_cube(float size);

#endif
