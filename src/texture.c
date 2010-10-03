#include "sc_texture.h"
#include "sc_path.h"
#include "sc_error.h"

static float default_texture_coordinates[8] =
    {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

static size_t
next_power_of_two(size_t value)
{
    value -= 1;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return value + 1;
}

sc_texture_t *
sc_texture_from_resource(const char *filename)
{
    char *path = sc_path_to_resource("textures", filename);
    SDL_Surface *surface = IMG_Load(path);
    if (!surface)
        sc_critical_error(SC_EGRAPHIC, "Unable to load texture");
    sc_free(path);
    sc_texture_t *rv = sc_texture_from_surface(surface);
    SDL_FreeSurface(surface);
    return rv;
}

sc_texture_t *
sc_texture_from_surface(SDL_Surface *img)
{
    int i, yl, yh;
    float scale[2] = {1.0f, 1.0f};
    GLuint tex;
    Uint8 *data, *line;
    SDL_Surface *stored_img = NULL;
    sc_texture_t *texture = sc_xalloc(sc_texture_t);

    texture->actual_width = img->w;
    texture->actual_height = img->h;
    texture->stored_width = next_power_of_two(img->w);
    texture->stored_height = next_power_of_two(img->h);

    /* figure out format */
    GLenum format;
    switch (img->format->BytesPerPixel) {
    case 4:
        format = (img->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
        break;
    case 3:
        format = (img->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
        break;
    default:
        sc_critical_error(SC_EGRAPHIC, "Unsupported texture depth");
    }

    /* flip image data because of flipped opengl coordinate system */
    data = (Uint8 *)(img->pixels);
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
    if (texture->stored_width != texture->actual_width ||
        texture->stored_height != texture->actual_height) {
        stored_img = SDL_CreateRGBSurface(img->flags,
            texture->stored_width, texture->stored_height,
            img->format->BitsPerPixel, img->format->Rmask,
            img->format->Gmask, img->format->Bmask,
            img->format->Amask);
        if (!stored_img)
            sc_critical_error(SC_EGRAPHIC, "Unable to resize texture");
        SDL_Rect rect = {0, 0, img->w, img->h};
        SDL_BlitSurface(img, &rect, stored_img, &rect);
        data = (Uint8 *)stored_img->pixels;
        scale[0] = (float)texture->actual_width / texture->stored_width;
        scale[1] = (float)texture->actual_height / texture->stored_height;
    }

    /* upload texture to graphics device */
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->stored_width,
                 texture->stored_height, 0, format, GL_UNSIGNED_BYTE,
                 data);

    /* remember default texture coordinates */
    for (i = 0; i < 8; i++)
        texture->coords[i] = default_texture_coordinates[i] * scale[i % 2];

    sc_free(stored_img);
    sc_free(line);
    return texture;
}

void
sc_free_texture(sc_texture_t *texture)
{
    if (!texture)
        return;
    glDeleteTextures(1, &texture->id);
    sc_free(texture);
}

void
sc_bind_texture(sc_texture_t *texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexCoordPointer(2, GL_FLOAT, 0, texture->coords);
}
