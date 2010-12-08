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
   on finalize the combined texture is uploaded to the graphics device.
   
   Something else important about the implementation is how it deals with
   texture bleeding.  Because a texture atlas works so that you will have
   to use fractions for addressing parts of the texture, you will always
   end up with rounding errors that cause visible artifacts.  This atlas
   counters these problems by cloning the edge pixels.  This means that
   per texture on the atlas, you will lose two rows and two columns (in
   pixels).

   */
#ifndef _INC_SC_ATLAS_H_
#define _INC_SC_ATLAS_H_

#include "sc_boot.h"
#include "sc_texture.h"

/* atlas is fragile and 100% implementation detail.  As such, the public
   interface is empty.  So that we don't have to cast things around, we just
   don't define that type and specify it in the struct instead. */
struct _sc_atlas;
typedef struct _sc_atlas sc_atlas_t;

/* creates a new atlas from a given size and filtering.  The size must
   be a power of two, even on systems that support it. */
sc_atlas_t *sc_new_atlas(size_t width, size_t height);

/* finalizes the atlas and uploads the texture into vram.  This also
   frees memory in the main memory for the surface */
int sc_finalize_atlas(sc_atlas_t *atlas);

/* frees the atlas.  This can be called both after and before finalizing */
void sc_free_atlas(sc_atlas_t *atlas);

/* adds a new texture from a resource to the atlas and returns it.  The
   returned texture must not be freed as it's part of the atlas.
   This function might fail in which case NULL is returned. */
const sc_texture_t *sc_atlas_add_from_resource(sc_atlas_t *atlas,
                                               const char *filename);

/* adds a new texture from an SDL surface and returns it.  The returned
   texture must not be freed as it's part of the atlas.
   This function might fail in which case NULL is returned. */
const sc_texture_t *sc_atlas_add_from_surface(sc_atlas_t *atlas,
                                              SDL_Surface *img);

/* returns the combined texture.  Requires that the atlas was finalized */
const sc_texture_t *sc_atlas_get_texture(const sc_atlas_t *atlas);

#endif
