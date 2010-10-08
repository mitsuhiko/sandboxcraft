#include "assert.h"

#include "sc_boot.h"
#include "sc_texture.h"
#include "sc_path.h"
#include "sc_math.h"

struct atlas_node;
struct atlas_node {
    struct atlas_node *left;
    struct atlas_node *right;
    sc_texture_t texture;
    size_t x;
    size_t y;
    size_t width;
    size_t height;
    int in_use;
};

typedef struct {
    struct atlas_node *root;
    SDL_Surface *surface;
    sc_texture_t *texture;
    GLint filtering;
    int finalized;
} sc_atlas_t;

/* we know what an atlas is, so don't let the header define it */
#define _SC_DONT_DEFINE_ATLAS
#include "sc_atlas.h"

static struct atlas_node *
new_node(size_t x, size_t y, size_t width, size_t height)
{
    struct atlas_node *rv = sc_xalloc(struct atlas_node);
    rv->left = NULL;
    rv->right = NULL;
    rv->x = x;
    rv->y = y;
    rv->width = width;
    rv->height = height;
    rv->in_use = 0;
    return rv;
}

static struct atlas_node *
insert_child_node(struct atlas_node *node, SDL_Surface *img)
{
    if (node->left) {
        struct atlas_node *rv;
        assert(node->right);
        rv = insert_child_node(node->left, img);
        if (!rv)
            rv = insert_child_node(node->right, img);
        return rv;
    }

    if (node->in_use || img->w > node->width || img->h > node->height)
        return NULL;

    if (img->w == node->width && img->h == node->height) {
        node->in_use = 1;
        return node;
    }

    if (node->width - img->w > node->height - img->h) {
        node->left = new_node(node->x, node->y, img->w + 2, node->height);
        node->right = new_node(node->x + img->w + 2, node->y,
                               node->width - img->w - 2,
                               node->height);
        node->left->left = new_node(node->x, node->y, img->w + 2, img->h + 2);
        node->left->right = new_node(node->x, node->y + img->w + 2,
                                     img->w + 2, node->height - img->h - 2);
    }
    else {
        node->left = new_node(node->x, node->y, node->width, img->h + 2);
        node->right = new_node(node->x, node->y + img->h + 2,
                               node->width, node->height - img->h - 2);
        node->left->left = new_node(node->x, node->y, img->w + 2, img->h + 2);
        node->left->right = new_node(node->x + img->w + 2, node->y,
                                     node->width - img->w - 2, img->h + 2);
    }

    node->left->left->in_use = 1;
    return node->left->left;
}

static void
update_texture_coords(struct atlas_node *node, sc_atlas_t *atlas)
{
    float atlas_width = (float)atlas->surface->w;
    float atlas_height = (float)atlas->surface->h;
    float u1 = (node->x + 1) / atlas_width;
    float v1 = (node->y + 1) / atlas_height;
    float u2 = (node->x + node->texture.width + 1) / atlas_width;
    float v2 = (node->y + node->texture.height + 1) / atlas_height;
    float new_coords[8] = {u1, v1, u2, v1, u2, v2, u1, v2};
    memcpy(node->texture.coords, new_coords, sizeof(float) * 8);
}

static void
sync_textures_recursive(struct atlas_node *node, sc_atlas_t *atlas)
{
    if (node->in_use) {
        node->texture.id = atlas->texture->id;
        node->texture.stored_width = atlas->texture->stored_width;
        node->texture.stored_height = atlas->texture->stored_height;
        update_texture_coords(node, atlas);
    }
    if (node->left)
        sync_textures_recursive(node->left, atlas);
    if (node->right)
        sync_textures_recursive(node->right, atlas);
}

static void
free_nodes_recursive(struct atlas_node *node)
{
    if (!node)
        return;
    free_nodes_recursive(node->left);
    free_nodes_recursive(node->right);
    sc_free(node);
}


sc_atlas_t *
sc_new_atlas(size_t width, size_t height, GLint filtering)
{
    sc_atlas_t *atlas = sc_xalloc(sc_atlas_t);
    width = sc_next_power_of_two(width);
    height = sc_next_power_of_two(height);
    atlas->surface = sc_memassert(SDL_CreateRGBSurface(
        SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0));
    atlas->filtering = filtering;
    atlas->root = new_node(0, 0, width, height);
    atlas->finalized = 0;
    return atlas;
}

sc_texture_t *
sc_atlas_add_from_resource(sc_atlas_t *atlas, const char *filename)
{
    assert(!atlas->finalized);
    char *path;
    sc_texture_t *rv;
    path = sc_path_to_resource("textures", filename);
    SDL_Surface *surface = IMG_Load(path);
    if (!surface) {
        sc_free(path);
        sc_set_error(SC_EGRAPHIC, path, 0, "Unable to load texture.  "
            "Tried to feed texture atlas.");
        return NULL;
    }
    rv = sc_atlas_add_from_surface(atlas, surface);
    if (!rv)
        sc_augment_error_context(filename, 0);
    sc_free(path);
    SDL_FreeSurface(surface);
    return rv;
}

sc_texture_t *
sc_atlas_add_from_surface(sc_atlas_t *atlas, SDL_Surface *img)
{
    SDL_Rect src_rect, dst_rect;
    struct atlas_node *rv;
    int i;
    assert(!atlas->finalized);
    
    if (!(rv = insert_child_node(atlas->root, img))) {
        sc_set_error(SC_EGRAPHIC, NULL, 0, "Atlas full. "
            "Cannot add further textures.  Expand size");
        return NULL;
    }

    /* blit flipped on the "wrong side" because we flip the whole thing
       over when we pass this over to opengl due to the mirrored coordinate
       system compared to a 2D image
       
       The blitting works in 5 parts:
       
       - First we blit the actual image to the center of our reserved block.
       - Then on each edge we blit the next best pixel row/column to the
         padding space.

       This is done to reduce artifacts caused by rounding errors.  If errors
       happen on blitting these are critical because we can't properly roll
       them back.
       */
#define TRY_BLIT(x1, y1, x2, y2, W, H) do { \
    src_rect.x = x1; src_rect.y = y1; \
    src_rect.w = W; src_rect.h = H; \
    dst_rect.x = x2; dst_rect.y = atlas->surface->h - img->h - (y2); \
    dst_rect.w = W; dst_rect.h = H; \
    if (SDL_BlitSurface(img, &src_rect, atlas->surface, &dst_rect) < 0) \
        sc_critical_error(SC_EGRAPHIC, __FILE__, __LINE__, \
            "Error on blitting: %s", SDL_GetError()); \
} while (0)

    /* actual texture */
    TRY_BLIT(0, 0, rv->x + 1, rv->y + 1, img->w, img->h);
    /* left edge */
    TRY_BLIT(0, 0, rv->x, rv->y + 1, 1, img->h);
    /* right edge */
    TRY_BLIT(img->w - 1, 0, rv->x + img->w + 1, rv->y + 1, 1, img->h);
    /* top edge */
    TRY_BLIT(0, img->h - 1, rv->x + 1, rv->y - img->h + 1, img->w, 1);
    /* bottom edge */
    TRY_BLIT(0, 0, rv->x + 1, rv->y + 2, img->w, 1);

    rv->texture.id = 0;
    rv->texture.shared = 1;
    rv->texture.width = img->w;
    rv->texture.height = img->h;

    return &rv->texture;
}

int
sc_finalize_atlas(sc_atlas_t *atlas)
{
    sc_texture_t *texture;
    assert(!atlas->finalized);
    texture = sc_texture_from_surface(atlas->surface, atlas->filtering);
    if (!texture)
        return 0;
    atlas->texture = texture;
    sync_textures_recursive(atlas->root, atlas);
    SDL_FreeSurface(atlas->surface);
    atlas->surface = NULL;
    atlas->finalized = 1;
    return 1;
}

void
sc_free_atlas(sc_atlas_t *atlas)
{
    if (!atlas)
        return;
    sc_free_texture(atlas->texture);
    SDL_FreeSurface(atlas->surface);
    free_nodes_recursive(atlas->root);
    sc_free(atlas);
}
