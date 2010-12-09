#include "sc_texture.h"
#include "sc_shading.h"
#include "sc_path.h"
#include "sc_error.h"
#include "sc_math.h"

static size_t
power_of_two_if_needed(size_t value)
{
    if (GLEE_ARB_texture_non_power_of_two)
        return value;
    return sc_next_power_of_two(value);
}

sc_texture_t *
sc_texture_from_resource(const char *filename, int mipmaps)
{
    char *path = sc_path_to_resource("textures", filename);
    sc_texture_t *rv;
    SDL_Surface *surface = IMG_Load(path);
    if (!surface) {
        sc_set_error(SC_ENOENT, path, 0, "Unable to load texture");
        sc_free(path);
        return NULL;
    }
    rv = sc_texture_from_surface(surface, mipmaps);
    if (!rv)
        sc_augment_error_context(filename, 0);
    sc_free(path);
    SDL_FreeSurface(surface);
    return rv;
}

sc_texture_t *
sc_texture_from_surface(SDL_Surface *img, int mipmaps)
{
    GLenum format;
    GLuint tex;
    uint8_t *data;
    SDL_Surface *stored_img = NULL;
    sc_texture_t *texture = sc_xalloc(sc_texture_t);

    texture->width = img->w;
    texture->height = img->h;
    texture->stored_width = power_of_two_if_needed(img->w);
    texture->stored_height = power_of_two_if_needed(img->h);
    texture->target = GL_TEXTURE_2D;
    texture->shared = 0;

    data = sc_prepare_surface_for_upload(img, &format);
    if (!data)
        return NULL;

    /* if stored size differ we have to blit the image to a new surface
       with the requested size before uploading */
    if (texture->stored_width != texture->width ||
        texture->stored_height != texture->height) {
        SDL_Rect rect = {0, 0, img->w, img->h};
        stored_img = SDL_CreateRGBSurface(img->flags,
            texture->stored_width, texture->stored_height,
            img->format->BitsPerPixel, img->format->Rmask,
            img->format->Gmask, img->format->Bmask,
            img->format->Amask);
        if (!stored_img) {
            sc_set_error(SC_EGRAPHIC, __FILE__, __LINE__, "Unable to resize texture");
            sc_free(texture);
            texture = NULL;
            goto bailout;
        }
        SDL_BlitSurface(img, &rect, stored_img, &rect);
        data = (uint8_t *)stored_img->pixels;
    }

    /* upload texture to graphics device */
    glGenTextures(1, &tex);
    glBindTexture(texture->target, tex);
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER,
        mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(texture->target, 0, GL_RGBA8, texture->stored_width,
                 texture->stored_height, 0, format, GL_UNSIGNED_BYTE,
                 data);

    /* build a bunch of mipmaps */
    if (mipmaps)
        glGenerateMipmap(texture->target);

    texture->id = tex;
    texture->index = -1;
    texture->off_x = 0;
    texture->off_y = 0;
    texture->width = img->w;
    texture->height = img->h;

bailout:
    sc_free(stored_img);
    glBindTexture(texture->target, 0);

    return texture;
}

void
sc_free_texture(sc_texture_t *texture)
{
    if (!texture)
        return;
    assert(!texture->shared);
    if (texture->id)
        glDeleteTextures(1, &texture->id);
    sc_free(texture);
}

void
sc_texture_bind(const sc_texture_t *texture)
{
    glActiveTexture(GL_TEXTURE0);
    sc_int_uniform(NULL, "sc_texture", 0);
    glBindTexture(texture->target, texture->id);
}

uint8_t *
sc_prepare_surface_for_upload(SDL_Surface *img, GLenum *format_out)
{
    int yl, yh;
    uint8_t *data, *line;
    GLenum format;

    /* figure out format */
    switch (img->format->BytesPerPixel) {
    case 4:
        format = (img->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
        break;
    case 3:
        format = (img->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
        break;
    default:
        sc_set_error(SC_EGRAPHIC, __FILE__, __LINE__, "Unsupported texture depth");
        return NULL;
    }

    /* flip image data because of flipped opengl coordinate system */
    data = (uint8_t *)img->pixels;
    line = sc_xmalloc(sizeof(uint8_t) * img->pitch);
    yl = 0;
    yh = img->h - 1;
    while (yl < yh) {
        memcpy(line, data + img->pitch * yl, img->pitch);
        memcpy(data + img->pitch * yl, data + img->pitch * yh, img->pitch);
        memcpy(data + img->pitch * yh, line, img->pitch);
        yl++, yh--;
    }
    sc_free(line);

    *format_out = format;
    return data;
}

int
sc_get_texcoord_dimension(GLenum target)
{
    switch (target) {
    case GL_TEXTURE_1D:
        return 1;
    case GL_TEXTURE_2D:
        return 2;
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_3D:
        return 3;
    default:
        assert(0);
        return 0;
    }
}
