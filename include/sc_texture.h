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
    size_t width;
    size_t height;
    float coords[8];
    int shared;
} sc_texture_t;

/* loads a texture from a resource.  This might fail */
sc_texture_t *sc_texture_from_resource(const char *filename, GLint filtering);

/* loads a texture from a surface.  This might fail */
sc_texture_t *sc_texture_from_surface(SDL_Surface *img, GLint filtering);

/* helper for texture uploading.  Figures out format and flips it */
uint8_t *sc_prepare_surface_for_upload(SDL_Surface *img, GLenum *format_out);

/* frees a texture.  Should be unbound */
void sc_free_texture(sc_texture_t *texture);

/* binds a texture to texture unit 0 and sets the shader uniform
   appropriately.  When we need more that one unit we have to update this. */
void sc_texture_bind(const sc_texture_t *texture);

/* unbinds a texture */
void sc_unbind_texture(void);

#endif
