/* implements texture arrays */
#ifndef _INC_SC_ARRAY_TEXTURE_H_
#define _INC_SC_ARRAY_TEXTURE_H_

#include "sc_boot.h"
#include "sc_texture.h"

struct _sc_arraytex;
typedef struct _sc_arraytex sc_arraytex_t;

/* creates a new array texture of a specific size */
sc_arraytex_t *sc_new_arraytex(size_t width, size_t height);

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
