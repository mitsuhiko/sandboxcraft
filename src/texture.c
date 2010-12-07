#include "sc_texture.h"
#include "sc_shading.h"
#include "sc_path.h"
#include "sc_error.h"
#include "sc_math.h"

static float default_texture_coordinates[8] =
    {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

static size_t
power_of_two_if_needed(size_t value)
{
    if (GLEE_ARB_texture_non_power_of_two)
        return value;
    return sc_next_power_of_two(value);
}

sc_texture_t *
sc_texture_from_resource(const char *filename, GLint filtering)
{
    char *path = sc_path_to_resource("textures", filename);
    sc_texture_t *rv;
    SDL_Surface *surface = IMG_Load(path);
    if (!surface) {
        sc_set_error(SC_ENOENT, path, 0, "Unable to load texture");
        sc_free(path);
        return NULL;
    }
    rv = sc_texture_from_surface(surface, filtering);
    if (!rv)
        sc_augment_error_context(filename, 0);
    sc_free(path);
    SDL_FreeSurface(surface);
    return rv;
}

sc_texture_t *
sc_texture_from_surface(SDL_Surface *img, GLint filtering)
{
    GLenum format;
    int i, yl, yh;
    float scale[2] = {1.0f, 1.0f};
    GLuint tex;
    Uint8 *data, *line;
    SDL_Surface *stored_img = NULL;
    sc_texture_t *texture = sc_xalloc(sc_texture_t);

    texture->width = img->w;
    texture->height = img->h;
    texture->stored_width = power_of_two_if_needed(img->w);
    texture->stored_height = power_of_two_if_needed(img->h);
    texture->shared = 0;

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
    data = (Uint8 *)img->pixels;
    line = sc_xmalloc(sizeof(Uint8) * img->pitch);
    yl = 0;
    yh = img->h - 1;
    while (yl < yh) {
        memcpy(line, data + img->pitch * yl, img->pitch);
        memcpy(data + img->pitch * yl, data + img->pitch * yh, img->pitch);
        memcpy(data + img->pitch * yh, line, img->pitch);
        yl++, yh--;
    }

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
        data = (Uint8 *)stored_img->pixels;
        scale[0] = (float)texture->width / texture->stored_width;
        scale[1] = (float)texture->height / texture->stored_height;
    }

    /* upload texture to graphics device */
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->stored_width,
                 texture->stored_height, 0, format, GL_UNSIGNED_BYTE,
                 data);

    /* remember default texture coordinates */
    for (i = 0; i < 8; i++)
        texture->coords[i] = default_texture_coordinates[i] * scale[i % 2];
    texture->id = tex;

bailout:
    sc_free(stored_img);
    sc_free(line);
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
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void
sc_unbind_texture(void)
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
