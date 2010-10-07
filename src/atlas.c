#include "assert.h"

#include "sc_boot.h"
#include "sc_texture.h"
#include "sc_path.h"

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
    union {
        SDL_Surface *surface;
        sc_texture_t *texture;
    } payload;
    GLint filtering;
    int finalized;
} sc_atlas_t;

/* we know what an atlas is, so don't let the header define it */
#define _SC_DONT_DEFINE_ATLAS
#include "sc_atlas.h"

static struct atlas_node *
make_new_node(size_t x, size_t y, size_t width, size_t height)
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
    size_t diffw, diffh;

    if (node->left /* || right */) {
        struct atlas_node *rv;
        rv = insert_child_node(node->left, img);
        if (!rv)
            rv = insert_child_node(node->right, img);
        return rv;
    }

    /* oh well, that node is used up */
    if (node->in_use)
        return NULL;

    /* but it might also just not fit the space */
    if (img->w > node->width || img->h > node->height)
        return NULL;

    /* perfect match */
    if (img->w == node->width && img->h == node->height) {
        node->in_use = 1;
        return node;
    }

    /* right or down? */
    diffw = node->width - img->w;
    diffh = node->height - img->h;

    if (diffw > diffh) {
        node->left = make_new_node(node->x, node->y, node->x + img->w - 1,
                                   node->y + node->width);
        node->right = make_new_node(node->x + img->w, node->y,
                                    node->x + node->width,
                                    node->y + node->height);
    }
    else {
        node->left = make_new_node(node->x, node->y, node->x + node->width,
                                   node->y + img->h - 1);
        node->right = make_new_node(node->x, node->y + img->h,
                                    node->x + node->width,
                                    node->y + node->height);
    }

    node->left->in_use = 1;
    return node->left;
}

static void
update_texture_coords(struct atlas_node *rv, sc_atlas_t *atlas)
{
    float u1 = rv->x / (float)atlas->payload.surface->w;
    float v1 = rv->y / (float)atlas->payload.surface->h;
    float u2 = (rv->x + rv->width) / (float)atlas->payload.surface->w;
    float v2 = (rv->y + rv->height) / (float)atlas->payload.surface->h;
    float new_coords[8] = {u1, v1, u2, v1, u2, v2, u1, v2};
    *rv->texture.coords = *new_coords;
}

static void
sync_textures_recursive(struct atlas_node *node, const sc_texture_t *texture)
{
    if (node->in_use) {
        node->texture.id = texture->id;
        node->texture.actual_width = node->width;
        node->texture.actual_height = node->height;
    }
    if (node->left)
        sync_textures_recursive(node->left, texture);
    if (node->right)
        sync_textures_recursive(node->left, texture);
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
    SDL_Surface *surface = sc_memassert(SDL_CreateRGBSurface(
        SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0));
    sc_atlas_t *atlas = sc_xalloc(sc_atlas_t);
    atlas->payload.surface = surface;
    atlas->filtering = filtering;
    atlas->root = make_new_node(0, 0, width, height);
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
    SDL_Rect src_rect = {0, 0, img->w, img->h};
    SDL_Rect dst_rect;
    struct atlas_node *rv;
    assert(!atlas->finalized);
    
    if (!(rv = insert_child_node(atlas->root, img))) {
        sc_set_error(SC_EGRAPHIC, NULL, 0, "Atlas full. "
            "Cannot add further textures.  Expand size");
        return NULL;
    }

    dst_rect.x = (Sint16)rv->x;
    dst_rect.y = (Sint16)rv->y;
    dst_rect.w = (Uint16)rv->width;
    dst_rect.h = (Uint16)rv->height;
    SDL_BlitSurface(img, &src_rect, atlas->payload.surface, &dst_rect);

    rv->texture.shared = 1;
    rv->texture.stored_width = img->w;
    rv->texture.stored_height = img->h;
    update_texture_coords(rv, atlas);

    return &rv->texture;
}

void
sc_finalize_atlas(sc_atlas_t *atlas)
{
    sc_texture_t *texture;
    assert(!atlas->finalized);
    texture = sc_texture_from_surface(atlas->payload.surface, atlas->filtering);
    SDL_FreeSurface(atlas->payload.surface);
    atlas->payload.texture = texture;
    sync_textures_recursive(atlas->root, texture);
    atlas->finalized = 1;
}

void
sc_free_atlas(sc_atlas_t *atlas)
{
    if (!atlas)
        return;
    if (atlas->finalized)
        sc_free_texture(atlas->payload.texture);
    else
        SDL_FreeSurface(atlas->payload.surface);
    free_nodes_recursive(atlas->root);
    sc_free(atlas);
}
