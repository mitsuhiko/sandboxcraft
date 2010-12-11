/* helper for texture loading and uploading */
#ifndef _INC_SC_TEXTURE_H_
#define _INC_SC_TEXTURE_H_

#include "sc_boot.h"

typedef GLuint sc_texid_t;

/* represents a texture that was uploaded to graphics device memory */
typedef struct {
    sc_texid_t id;          /* id of the texture on the device */
    size_t stored_width;    /* texture width */
    size_t stored_height;   /* texture height */
    size_t off_x;           /* offset in x direction in pixels */
    size_t off_y;           /* offset in y direction in pixels */
    size_t width;           /* width of the surface on the texture */
    size_t height;          /* height of the surface on the texture */
    int index;              /* index in a texture array (-1 if not array) */
    GLenum target;          /* the texture target (GL_TEXTURE_2D) */
    int shared;             /* if this texture is shared with other
                               texture object on the graphics device */
} sc_texture_t;

/* loads a 2d texture from a resource.  This might fail */
sc_texture_t *sc_texture_from_resource(const char *filename, int mipmaps);

/* loads a 2d texture from a surface.  This might fail */
sc_texture_t *sc_texture_from_surface(SDL_Surface *img, int mipmaps);

/* frees a texture.  Should be unbound */
void sc_free_texture(sc_texture_t *texture);

/* binds a texture to texture unit 0 and sets the shader uniform
   appropriately.  When we need more that one unit we have to update this. */
void sc_texture_bind(const sc_texture_t *texture);

/* helper for texture uploading.  Figures out format and flips it into a
   new surface. */
SDL_Surface *sc_prepare_surface_for_upload(SDL_Surface *img, GLenum *format_out);

/* returns the dimension of texture coordinates for this target */
int sc_get_texcoord_dimension(GLenum target);

/* enables ansitropic filtering for a texture target as specified in the
   config.  This automatically falls back to supported levels. */
void sc_filter_ansitropic(GLenum target);

#endif
