/* implements texture arrays */
#ifndef _INC_SC_ARRAY_TEXTURE_H_
#define _INC_SC_ARRAY_TEXTURE_H_

#include "sc_boot.h"
#include "sc_texture.h"

/* usage modes for the array texture.  Because array textures are currently
   used to generate the world of sandboxcraft it has some extra usage modes
   over atlases and raw textures.  Namely that they can scale up and down
   with nearest neighbor when SC_ARRAYTEX_NEAREST and SC_ARRAYTEX_MIPMAPS
   are set. */
#define SC_ARRAYTEX_NEAREST 1   /* scale images with nearest neigbour */
#define SC_ARRAYTEX_MIPMAPS 2   /* generate mipmaps */

struct _sc_arraytex;
typedef struct _sc_arraytex sc_arraytex_t;

/* creates a new array texture of a specific size */
sc_arraytex_t *sc_new_arraytex(size_t width, size_t height, int flags);

/* frees the texture again */
void sc_free_arraytex(sc_arraytex_t *arr);

/* adds a texture to the array from a file or NULL on error. */
const sc_texture_t *sc_arraytex_add_from_resource(sc_arraytex_t *arr, const char *filename);

/* adds a new texture to the array texture and returns it or NULL on error. */
const sc_texture_t *sc_arraytex_add_from_surface(sc_arraytex_t *arr, SDL_Surface *img);

/* finalizes the array texture */
int sc_arraytex_finalize(sc_arraytex_t *arr);

/* returns the 3D texture.  Requires that the array texture was finalized */
const sc_texture_t *sc_arraytex_get_texture(const sc_arraytex_t *arr);

#endif
