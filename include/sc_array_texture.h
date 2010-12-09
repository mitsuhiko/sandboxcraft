/* implements texture arrays */
#ifndef _INC_SC_ARRAY_TEXTURE_H_
#define _INC_SC_ARRAY_TEXTURE_H_

#include "sc_boot.h"
#include "sc_texture.h"

struct _sc_arraytex;
typedef struct _sc_arraytex sc_arraytex_t;

/* creates a new array texture of a specific size */
sc_arraytex *sc_new_arraytex(size_t width, size_t height, size_t slices);

/* frees the texture again */
void sc_free_arraytex(sc_arraytex *tex);

#endif
