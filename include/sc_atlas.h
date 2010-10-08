/* implements a basic texture atlas

   The idea of such an atlas is that a bunch of images from the file
   system are uploaded into one combined texture with the best packing
   possible.  While the algorithm applied here is pretty basic it still
   does the trick.

   Why atlasses?  Texture switches are expensive and that way only the
   coordinates have to be reassigned before drawing.  The textures
   returned from the atlas'es add*() methods return texture objects with
   the correct coordinates set.

   Notes on memory management: when the atlas is deallocated the texture
   is removed from the graphics device and all textures are freed as well.
   This means that no texture returned from the atlas must be used after
   that point.  On the other hand you also must not attempt to free the
   textures returned from the atlas as these are actually not allocated
   dynamically but part of the internal atlas memory.
   
   The atlas has to be finalized before any texture is used because only
   on finalize the combined texture is uploaded to the graphics device. */
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

/* creates a new atlas from a given size and filtering.  The size should
   be a power of two because otherwise the texture subsystem rounds it up
   but too late to take advantage of the extra size. */
sc_atlas_t *sc_new_atlas(size_t width, size_t height, GLint filtering);

/* finalizes the atlas and uploads the texture into vram.  This also
   frees memory in the main memory for the surface */
int sc_finalize_atlas(sc_atlas_t *atlas);

/* frees the atlas.  This can be called both after and before finalizing */
void sc_free_atlas(sc_atlas_t *atlas);

/* adds a new texture from a resource to the atlas and returns it.  The
   returned texture must not be freed as it's part of the atlas.
   This function might fail in which case NULL is returned. */
sc_texture_t *sc_atlas_add_from_resource(sc_atlas_t *atlas,
                                         const char *filename);

/* adds a new texture from an SDL surface and returns it.  The returned
   texture must not be freed as it's part of the atlas.
   This function might fail in which case NULL is returned. */
sc_texture_t *sc_atlas_add_from_surface(sc_atlas_t *atlas,
                                        SDL_Surface *img);

#endif
