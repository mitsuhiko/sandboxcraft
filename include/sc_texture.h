/* helper for texture loading and uploading */
#ifndef _INC_SC_TEXTURE_H_
#define _INC_SC_TEXTURE_H_

#include "sc_boot.h"

typedef GLuint sc_texid_t;

/* represents a texture that was uploaded to graphics device memory */
typedef struct {
    sc_texid_t id;
    size_t stored_width;
    size_t stored_height;
    size_t actual_width;
    size_t actual_height;
    float coords[8];
} sc_texture_t;

sc_texture_t *sc_texture_from_resource(const char *filename);
sc_texture_t *sc_texture_from_surface(SDL_Surface *img);
void sc_free_texture(sc_texture_t *texture);
void sc_bind_texture(sc_texture_t *texture);

#endif
