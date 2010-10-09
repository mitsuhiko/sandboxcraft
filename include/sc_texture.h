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

/* frees a texture.  If it's bound, it will be unbound first */
void sc_free_texture(sc_texture_t *texture);

/* binds a texture.  In case this texture is shared and the last bound
   texture points to the same atlas in video card memory, only the
   coordinates are sent.  This always binds a GL_TEXTURE_2D. */
void sc_bind_texture(const sc_texture_t *texture);

/* unbinds a texture.  Always use this instead of glBindTexture because
   otherwise internal optimizations break and you end up with really
   scary and weird results. */
void sc_unbind_texture(void);

/* sends the texture coordinates to the device */
void sc_send_texture_coordinates(const sc_texture_t *texture);

#endif
