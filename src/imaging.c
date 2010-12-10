#include "sc_imaging.h"

SDL_Surface *
sc_resize_surface_nearest(const SDL_Surface *img, size_t width, size_t height)
{
    size_t x, y, ix, iy;
    int d = img->format->BytesPerPixel;

    SDL_Surface *rv = sc_memassert(SDL_CreateRGBSurface(
        img->flags, width, height, img->format->BitsPerPixel,
        img->format->Rmask, img->format->Gmask,
        img->format->Bmask, img->format->Amask));

    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++) {
            ix = (size_t)((float)x / width * img->w);
            iy = (size_t)((float)y / height * img->h);
            memcpy((char *)rv->pixels + rv->pitch * y + d * x,
                   (const char *)img->pixels + img->pitch * iy + d * ix, d);
        }

    return rv;
}
