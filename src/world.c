#include <assert.h>
#include <stdlib.h>

#include "sc_engine.h"
#include "sc_world.h"
#include "sc_frustum.h"
#include "sc_primitives.h"

#define BLOCK_SIZE 20.0f

sc_world_t *
sc_new_world(void)
{
    sc_world_t *world = sc_xalloc(sc_world_t);
    world->root = sc_new_chunk_node();
    return world;
}

void
sc_free_world(sc_world_t *world)
{
    if (!world)
        return;
    sc_free_chunk_node(world->root);
}

static sc_chunk_node_t *
find_vbo_node(sc_world_t *world, int x, int y, int z)
{
    int size;
    sc_chunk_node_t *node, *child;

    if (x >= SC_CHUNK_RESOLUTION || x < 0 ||
        y >= SC_CHUNK_RESOLUTION || y < 0 ||
        z >= SC_CHUNK_RESOLUTION || z < 0)
        return NULL;

    /* Find the block in the octree */
    node = world->root;
    size = SC_CHUNK_RESOLUTION;
    while (1) {
        size_t idx;
        size /= 2;
        idx = !!(x & size) << 2 | !!(y & size) << 1 | !!(z & size);
        child = node->children[idx];
        if (!child) {
            child = sc_new_chunk_node();
            node->children[idx] = child;
            /* remember the block when we create a new child */
            child->block = node->block;
        }
        node = child;
        if (size == SC_CHUNK_VBO_SIZE)
            break;
    }

    return node;
}

const sc_block_t *
sc_world_get_block(sc_world_t *world, int x, int y, int z)
{
    int size;
    sc_chunk_node_t *node, *child;

    /* stuff outside of the cube is air */
    if (x >= SC_CHUNK_RESOLUTION || x < 0 ||
        y >= SC_CHUNK_RESOLUTION || y < 0 ||
        z >= SC_CHUNK_RESOLUTION || z < 0)
        return sc_get_block(SC_BLOCK_AIR);

    /* locate the block in the octree */
    node = world->root;
    size = SC_CHUNK_RESOLUTION;
    while (size != 1) {
        size_t idx;
        size /= 2;
        idx = !!(x & size) << 2 | !!(y & size) << 1 | !!(z & size);
        child = node->children[idx];
        if (!child)
            break;
        node = child;
    }

    return node->block;
}

int
sc_world_set_block(sc_world_t *world, int x, int y, int z,
                   const sc_block_t *block)
{
    int size;
    sc_chunk_node_t *node, *child;

    if (x >= SC_CHUNK_RESOLUTION || x < 0 ||
        y >= SC_CHUNK_RESOLUTION || y < 0 ||
        z >= SC_CHUNK_RESOLUTION || z < 0)
        return 0;

    /* Find the block in the octree */
    node = world->root;
    size = SC_CHUNK_RESOLUTION;
    while (size != 1) {
        size_t idx;
        size /= 2;
        idx = !!(x & size) << 2 | !!(y & size) << 1 | !!(z & size);
        child = node->children[idx];
        if (!child) {
            child = sc_new_chunk_node();
            node->children[idx] = child;
            /* remember the block when we create a new child */
            child->block = node->block;
        }
        node = child;
        if (size == SC_CHUNK_VBO_SIZE)
            node->dirty = 1;
    }

    /* helper macro to mark other vbos as dirty */
#define MARK_DIRTY(X, Y, Z) do { \
    sc_chunk_node_t *node = find_vbo_node(world, X, Y, Z); \
    if (node) \
        node->dirty = 1; \
} while (0)

    /* in case we replace air for non air at an edge block we have to be
       extra careful and update the dirty flags of the blocks nearby. */
    if ((node->block->type == SC_BLOCK_AIR) != (block->type == SC_BLOCK_AIR)) {
        if ((x + 1) % SC_CHUNK_RESOLUTION == 0) MARK_DIRTY(x + 1, y, z);
        if ((x - 1) % SC_CHUNK_RESOLUTION == 0) MARK_DIRTY(x - 1, y, z);
        if ((y + 1) % SC_CHUNK_RESOLUTION == 0) MARK_DIRTY(x, y + 1, z);
        if ((y - 1) % SC_CHUNK_RESOLUTION == 0) MARK_DIRTY(x, y - 1, z);
        if ((z + 1) % SC_CHUNK_RESOLUTION == 0) MARK_DIRTY(x, y, z + z);
        if ((z - 1) % SC_CHUNK_RESOLUTION == 0) MARK_DIRTY(x, y, z - 1);
    }

    node->block = block;
    return 1;
}

const sc_block_t *
sc_world_get_block_by_pixel(sc_world_t *world, int sx, int sy,
                            int *x, int *y, int *z)
{
    int bx, by, bz;
    sc_vec3_t hit;

    if (!sc_engine_unproject(&hit, sx, sy))
        return NULL;

    bx = (int)(hit.x + BLOCK_SIZE / 2) / BLOCK_SIZE;
    by = (int)(hit.y + BLOCK_SIZE / 2) / BLOCK_SIZE;
    bz = (int)(hit.z + BLOCK_SIZE / 2) / BLOCK_SIZE;

    if (bx >= SC_CHUNK_RESOLUTION || bx < 0 ||
        by >= SC_CHUNK_RESOLUTION || by < 0 ||
        bz >= SC_CHUNK_RESOLUTION || bz < 0)
        return NULL;

    *x = bx;
    *y = by;
    *z = bz;
    return sc_world_get_block(world, bx, by, bz);
}

static void
walk_chunk(sc_world_t *world, const sc_chunk_node_t **children,
           const sc_block_t *block, int x, int y, int z, size_t size,
           sc_chunk_walk_cb cb, void *closure)
{
    int i;

    if (!cb(world, block, x, y, z, size, closure) && (size /= 2) < 1)
        return;

    if ((size /= 2) < 1)
        return;

    for (i = 0; i < 8; i++) {
        const sc_chunk_node_t *child = children ? children[i] : NULL;
        walk_chunk(world, (const sc_chunk_node_t **)(child ? child->children
                                                           : NULL),
                   child ? child->block : block,
                   x + ((i & 4) ? size : 0),
                   y + ((i & 2) ? size : 0),
                   z + ((i & 1) ? size : 0),
                   size, cb, closure);
    }
}

void
sc_walk_world(sc_world_t *world, sc_chunk_walk_cb cb, void *closure)
{
    walk_chunk(world, (const sc_chunk_node_t **)world->root->children,
               world->root->block, 0, 0, 0, SC_CHUNK_RESOLUTION, cb, closure);
}

static int
contents_visible(const sc_frustum_t *frustum, int x, int y, int z, size_t size)
{
    sc_vec3_t vec1, vec2;
    sc_vec3_set(&vec1, BLOCK_SIZE * x - BLOCK_SIZE / 2,
                       BLOCK_SIZE * y - BLOCK_SIZE / 2,
                       BLOCK_SIZE * z - BLOCK_SIZE / 2);
    sc_vec3_set(&vec2, BLOCK_SIZE * size, BLOCK_SIZE * size, BLOCK_SIZE * size);
    sc_vec3_add(&vec2, &vec2, &vec1);
    return sc_frustum_test_aabb(frustum, &vec1, &vec2) >= 0;
}

static int
draw_if_visible(sc_world_t *world, const sc_block_t *block, int x, int y,
                int z, size_t size, void *closure)
{
    const sc_frustum_t *frustum = closure;
    if (!contents_visible(frustum, x, y, z, size))
        return 0;

    /* we reached the level of the octree where vbos are stored.  Draw
       that and stop recursing.  sc_world_get_vbo will automatically
       update the vbo if necessary. */
    if (size == SC_CHUNK_VBO_SIZE) {
        const sc_vbo_t *vbo = sc_world_get_vbo(world, x, y, z);
        assert(vbo);
        sc_bind_texture(sc_blocks_get_atlas_texture());
        sc_vbo_draw(vbo);
        return 0;
    }

    return 1;
}

static void
update_vbo(sc_world_t *world, sc_chunk_node_t *node, int min_x, int min_y,
           int min_z, size_t size)
{
    const sc_block_t *block;
    int x, y, z;
    int max_x = min_x + size;
    int max_y = min_y + size;
    int max_z = min_z + size;

    if (node->vbo)
        sc_reuse_vbo(node->vbo);
    else
        node->vbo = sc_new_vbo();

#define IS_AIR(X, Y, Z) \
    (sc_world_get_block(world, X, Y, Z)->type == SC_BLOCK_AIR)

#define ADD_PLANE(Side) do { \
    sc_cube_add_ ##Side## _plane(node->vbo, BLOCK_SIZE, x * BLOCK_SIZE, \
                                 y * BLOCK_SIZE, z * BLOCK_SIZE); \
    sc_vbo_update_texcoords_from_texture_range( \
        node->vbo, node->vbo->vertices - 6, node->vbo->vertices, \
        block->texture); \
} while (0)

    /* we add all sides of each cube to the vbo that touch air */
    for (z = min_z; z < max_z; z++)
        for (y = min_y; y < max_y; y++)
            for (x = min_x; x < max_x; x++) {
                block = sc_world_get_block(world, x, y, z);
                if (block->type == SC_BLOCK_AIR)
                    continue;

                if (IS_AIR(x - 1, y, z)) ADD_PLANE(left);
                if (IS_AIR(x + 1, y, z)) ADD_PLANE(right);
                if (IS_AIR(x, y - 1, z)) ADD_PLANE(bottom);
                if (IS_AIR(x, y + 1, z)) ADD_PLANE(top);
                if (IS_AIR(x, y, z - 1)) ADD_PLANE(back);
                if (IS_AIR(x, y, z + 1)) ADD_PLANE(front);
            }

    node->dirty = 0;
    sc_finalize_vbo(node->vbo);
}

const sc_vbo_t *
sc_world_get_vbo(sc_world_t *world, int x, int y, int z)
{
    sc_chunk_node_t *node = find_vbo_node(world, x, y, z);
    if (!node)
        return NULL;
    if (!node->vbo || node->dirty)
        update_vbo(world, node, x - x % SC_CHUNK_VBO_SIZE,
                   y - y % SC_CHUNK_VBO_SIZE, z - z % SC_CHUNK_VBO_SIZE,
                   SC_CHUNK_VBO_SIZE);
    return node->vbo;
}

void
sc_world_draw(sc_world_t *world)
{
    sc_frustum_t frustum;
    sc_get_current_frustum(&frustum);
    sc_walk_world(world, draw_if_visible, &frustum);
}

sc_chunk_node_t *
sc_new_chunk_node(void)
{
    sc_chunk_node_t *node = sc_xalloc(sc_chunk_node_t);
    memset(node->children, 0, sizeof(node->children));
    node->block = sc_get_block(SC_BLOCK_AIR);
    node->vbo = NULL;
    node->dirty = 0;
    return node;
}

void
sc_free_chunk_node(sc_chunk_node_t *node)
{
    size_t i;
    if (!node)
        return;
    for (i = 0; i < 8; i++)
        sc_free_chunk_node(node->children[i]);
    if (node->vbo)
        sc_free_vbo(node->vbo);
    sc_free(node);
}
