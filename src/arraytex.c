#include "sc_arraytex.h"
#include "sc_strbuf.h"
#include "sc_list.h"
#include "sc_path.h"
#include "sc_imaging.h"
#include "sc_math.h"

#define TARGET GL_TEXTURE_2D_ARRAY
#define ASSERT_FINALIZED(X) assert(X->texture)
#define ASSERT_NOT_FINALIZED(X) assert(!X->texture)

struct _sc_arraytex {
    size_t width;
    size_t height;
    size_t slices;
    GLenum format;
    sc_strbuf_t **buffers;
    sc_list_t *textures;
    sc_texture_t *texture;
    int flags;
    size_t mipmap_levels;
};

sc_arraytex_t *
sc_new_arraytex(size_t width, size_t height, int flags)
{
    int i;
    sc_arraytex_t *rv = sc_xalloc(sc_arraytex_t);
    /* because we do not support resizing of textures we have to ensure
       that what we create here is a power of two */
    assert(sc_is_power_of_two(width));
    assert(sc_is_power_of_two(height));
    rv->width = width;
    rv->height = height;
    rv->slices = 0;
    rv->texture = NULL;
        
    /* XXX: nearest neighbour mipmap generation appears broken on windows.
        this might be caused by wrong mipmap levels, a memory corruption
        in the algorithm or unexpected pitch. */
#if SC_PLATFORM != SC_PLATFORM_WINDOWS
    if (flags & SC_ARRAYTEX_MIPMAPS && flags & SC_ARRAYTEX_NEAREST)
        rv->mipmap_levels = sc_intlog2(sc_max(width, height)) + 1;
    else
        rv->mipmap_levels = 1;
#else
    rv->mipmap_levels = 1;
#endif

    rv->buffers = sc_xmalloc(sizeof(sc_strbuf_t *) * rv->mipmap_levels);
    for (i = 0; i < rv->mipmap_levels; i++)
        rv->buffers[i] = sc_new_strbuf();
    rv->textures = sc_new_list();
    rv->flags = flags;
    return rv;
}

void
sc_free_arraytex(sc_arraytex_t *arr)
{
    int i;
    if (!arr)
        return;
    for (i = 0; i < arr->textures->size; i++)
        sc_free(arr->textures->items[i]);
    if (arr->buffers) {
        for (i = 0; i < arr->mipmap_levels; i++)
            sc_free_strbuf(arr->buffers[i]);
        sc_free(arr->buffers);
    }
    sc_free_list(arr->textures);
    sc_free_texture(arr->texture);
    sc_free(arr);
}

const sc_texture_t *
sc_arraytex_add_from_resource(sc_arraytex_t *arr, const char *filename)
{
    char *path;
    const sc_texture_t *rv;
    SDL_Surface *surface;
    ASSERT_NOT_FINALIZED(arr);
    path = sc_path_to_resource("textures", filename);
    surface = IMG_Load(path);
    if (!surface) {
        sc_free(path);
        sc_set_error(SC_EGRAPHIC, path, 0, "Unable to load texture.  "
            "Tried to feed texture array");
        return NULL;
    }
    rv = sc_arraytex_add_from_surface(arr, surface);
    if (!rv)
        sc_augment_error_context(filename, 0);
    sc_free(path);
    SDL_FreeSurface(surface);
    return rv;
}

static void
append_image(sc_strbuf_t *buffer, SDL_Surface *img)
{
    size_t i;
    size_t span = img->w * img->format->BytesPerPixel;
    size_t size = img->h * span;

    /* if the size is the pitch, we can copy all bytes over directly as
       the files are next to each other in the file */
    if (size == img->pitch) {
        sc_strbuf_append_bytes(buffer, (const char *)img->pixels, size);
        return;
    }

    /* seems like SDL decided to pad the image, we have to copy line for
       line over and skip some pieces */
    for (i = 0; i < img->h; i++)
        sc_strbuf_append_bytes(buffer, (const char *)img->pixels + (img->pitch * i), span);
}

const sc_texture_t *
sc_arraytex_add_from_surface(sc_arraytex_t *arr, SDL_Surface *img)
{
    size_t i;
    size_t w = arr->width, h = arr->height;
    sc_texture_t *rv;
    SDL_Surface *helper_img, *previous_img;
    GLenum format;
    ASSERT_NOT_FINALIZED(arr);

    img = sc_prepare_surface_for_upload(img, &format);
    if (!img)
        return NULL;

    /* we currently do not support resizing of the images on adding, so
       let's just make sure we add only images of the same size */
    if (img->w != arr->height || img->h != arr->height) {
        if (!(arr->flags & SC_ARRAYTEX_NEAREST)) {
            SDL_FreeSurface(img);
            sc_set_error(SC_EGRAPHIC, __FILE__, __LINE__,
                         "Texture size (%dx%d) does not match array size "
                         "(%dx%d)", (int)img->w, (int)img->h,
                         (int)arr->width, (int)arr->height);
            return NULL;
        }

        helper_img = img;
        img = sc_resize_surface_nearest(img, arr->width, arr->height);
        SDL_FreeSurface(helper_img);
    }

    /* same goes for the format.  All images added to the array have to
       be of the same format.  Currently not a big deal as we are only
       having RGBA images anyways, but it's important to check. */
    if (arr->slices == 0)
        arr->format = format;
    else if (arr->format != format) {
        SDL_FreeSurface(img);
        sc_set_error(SC_EGRAPHIC, __FILE__, __LINE__,
                     "Texture format mismatch when adding new texture "
                     "to texture array.  Array format is %d, added "
                     "format is %d.", arr->format, format);
        return NULL;
    }

    rv = sc_xalloc(sc_texture_t);
    rv->id = 0; /* set later, we don't know it yet */
    rv->stored_width = arr->width;
    rv->stored_height = arr->height;
    rv->off_x = 0;
    rv->off_y = 0;
    rv->index = arr->slices++;
    rv->width = arr->width;
    rv->height = arr->height;
    rv->target = TARGET;
    rv->shared = 1;

    /* first level is not resized */
    append_image(arr->buffers[0], img);

    /* all other levels are scaled down */
    helper_img = NULL;
    previous_img = NULL;
    for (i = 1; i < arr->mipmap_levels; i++) {
        if (w > 1) w /= 2;
        if (h > 1) h /= 2;
        helper_img = sc_resize_surface_nearest(
            previous_img ? previous_img : img, w, h);
        SDL_FreeSurface(previous_img);
        append_image(arr->buffers[i], helper_img);
        previous_img = helper_img;
    }

    SDL_FreeSurface(img);
    SDL_FreeSurface(helper_img);
    sc_list_append(arr->textures, rv);

    return rv;
}

int
sc_arraytex_finalize(sc_arraytex_t *arr)
{
    sc_texture_t *texture;
    char *data;
    int i;
    int use_mipmaps = arr->flags & SC_ARRAYTEX_MIPMAPS;
    ASSERT_NOT_FINALIZED(arr);
    assert(arr->slices > 0);

    texture = sc_xalloc(sc_texture_t);
    glGenTextures(1, &texture->id);
    glBindTexture(TARGET, texture->id);
    /* use ansitropic filtering if possible */
    if (GLEE_EXT_texture_filter_anisotropic)
        glTexParameterf(TARGET, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
    glTexParameteri(TARGET, GL_TEXTURE_MIN_FILTER,
                    use_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(TARGET, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(TARGET, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(TARGET, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* base level */
    data = sc_free_strbuf_and_get_contents(arr->buffers[0], NULL);
    glTexImage3D(TARGET, 0, GL_RGBA8, arr->width, arr->height,
                 arr->slices, 0, arr->format, GL_UNSIGNED_BYTE, data);
    sc_free(data);

    /* mipmap levels if given */
    if (use_mipmaps) {
        /* we haven't any mipmaps generated, let the graphics device do that
           for us.  Currently this is the case if we are using nearest
           neighbor mipmaps which we have to generate on the CPU. */
        if (arr->mipmap_levels == 1) {
            glGenerateMipmap(TARGET);

        /* otherwise upload the manually generated ones now */
        } else {
            size_t w = arr->width;
            size_t h = arr->height;
            for (i = 1; i < arr->mipmap_levels; i++) {
                if (w > 1) w /= 2;
                if (h > 1) h /= 2;
                data = sc_free_strbuf_and_get_contents(arr->buffers[i], NULL);
                glTexImage3D(TARGET, i, GL_RGBA8, w, h, arr->slices, 0,
                             arr->format, GL_UNSIGNED_BYTE, data);
                sc_free(data);
            }
        }
    }

    sc_free(arr->buffers);
    arr->buffers = NULL;
    texture->stored_width = arr->width;
    texture->stored_height = arr->height;
    texture->off_x = 0;
    texture->off_y = 0;
    texture->width = arr->width;
    texture->height = arr->height;
    texture->target = TARGET;
    texture->shared = 0;
    texture->index = -1;

    arr->texture = texture;
    return 1;
}

const sc_texture_t *
sc_arraytex_get_texture(const sc_arraytex_t *arr)
{
    ASSERT_FINALIZED(arr);
    return arr->texture;
}
