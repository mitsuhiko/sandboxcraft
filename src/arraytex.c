#include "sc_arraytex.h"
#include "sc_strbuf.h"
#include "sc_list.h"
#include "sc_path.h"
#include "sc_math.h"

#define TARGET GL_TEXTURE_2D_ARRAY
#define ASSERT_FINALIZED(X) assert(X->texture)
#define ASSERT_NOT_FINALIZED(X) assert(!X->texture)

struct _sc_arraytex {
    size_t width;
    size_t height;
    size_t slices;
    GLenum format;
    sc_strbuf_t *buffer;
    sc_list_t *textures;
    sc_texture_t *texture;
    int mipmaps;
};

sc_arraytex_t *
sc_new_arraytex(size_t width, size_t height)
{
    sc_arraytex_t *rv = sc_xalloc(sc_arraytex_t);
    /* because we do not support resizing of textures we have to ensure
       that what we create here is a power of two */
    assert(sc_is_power_of_two(width));
    assert(sc_is_power_of_two(height));
    rv->width = width;
    rv->height = height;
    rv->slices = 0;
    rv->texture = NULL;
    rv->buffer = sc_new_strbuf();
    rv->textures = sc_new_list();
    rv->mipmaps = 1;
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
    sc_free_list(arr->textures);
    sc_free_strbuf(arr->buffer);
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

const sc_texture_t *
sc_arraytex_add_from_surface(sc_arraytex_t *arr, SDL_Surface *img)
{
    uint8_t *data;
    size_t size;
    sc_texture_t *rv;
    GLenum format;
    ASSERT_NOT_FINALIZED(arr);

    data = sc_prepare_surface_for_upload(img, &format);
    if (!data)
        return NULL;

    /* we currently do not support resizing of the images on adding, so
       let's just make sure we add only images of the same size */
    if (img->w != arr->height || img->h != arr->height) {
        sc_set_error(SC_EGRAPHIC, __FILE__, __LINE__,
                     "Texture size (%dx%d) does not match array size (%dx%d)",
                     (int)img->w, (int)img->h, (int)arr->width,
                     (int)arr->height);
        return NULL;
    }

    /* same goes for the format.  All images added to the array have to
       be of the same format.  Currently not a big deal as we are only
       having RGBA images anyways, but it's important to check. */
    if (arr->slices == 0)
        arr->format = format;
    else if (arr->format != format) {
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

    size = arr->width * arr->height * 4;
    sc_strbuf_append_bytes(arr->buffer, img->pixels, size);
    sc_list_append(arr->textures, rv);

    return rv;
}

int
sc_arraytex_finalize(sc_arraytex_t *arr)
{
    sc_texture_t *texture;
    char *data;
    ASSERT_NOT_FINALIZED(arr);
    assert(arr->slices > 0);

    data = sc_free_strbuf_and_get_contents(arr->buffer, NULL);
    arr->buffer = NULL;

    texture = sc_xalloc(sc_texture_t);
    glGenTextures(1, &texture->id);
    glBindTexture(TARGET, texture->id);
    if (arr->mipmaps)
        glTexParameteri(TARGET, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else
        glTexParameteri(TARGET, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(TARGET, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage3D(TARGET, 0, GL_RGBA8, arr->width, arr->height,
                 arr->slices, 0, arr->format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(TARGET);
    sc_free(data);

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
