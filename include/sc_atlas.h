/* implements a basic texture atlas */
#ifndef _INC_SC_ATLAS_H_
#define _INC_SC_ATLAS_H_

#include "sc_boot.h"
#include "sc_texture.h"

/* atlas is fragile and 100% implementation detail.  As such, the public
   interface is empty.  So that we don't have to cast things around, we just
   don't define that type and specify it in the struct instead. */
#ifndef _SC_DONT_DEFINE_ATLAS
typedef struct {} sc_atlas_t;
#endif

sc_atlas_t *sc_new_atlas(size_t width, size_t height, GLint filtering);
void sc_finalize_atlas(sc_atlas_t *atlas);
void sc_free_atlas(sc_atlas_t *atlas);

sc_texture_t *sc_atlas_add_from_resource(sc_atlas_t *atlas,
                                         const char *filename);
sc_texture_t *sc_atlas_add_from_surface(sc_atlas_t *atlas,
                                        SDL_Surface *img);

#endif
