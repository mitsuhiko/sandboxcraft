/* imaging helpers.  This currently operates on SDL surfaces */
#ifndef _INC_SC_IMAGING_H_
#define _INC_SC_IMAGING_H_

#include "sc_boot.h"

/* helper function that resizes an SDL surface with nearest neighbour
   interpolation which is basically no interpolation.  This is currently
   the only implemented resizing not because it's simplest (though it is)
   but because the intended game look for the time being is minecraft
   inspired which uses blocky graphics and this is what we want to
   retain here. */
SDL_Surface *sc_resize_surface_nearest(const SDL_Surface *img,
                                       size_t width, size_t height);

#endif
