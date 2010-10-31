#include <assert.h>
#include <stdlib.h>

#include "sc_engine.h"
#include "sc_world.h"
#include "sc_frustum.h"

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

const sc_block_t *
sc_world_get_block(sc_world_t *world, int x, int y, int z)
{
    int size;
    sc_chunk_node_t *node, *child;

    /* stuff outside of the cube is air */
    if (x > SC_CHUNK_RESOLUTION || x < 0 ||
        y > SC_CHUNK_RESOLUTION || y < 0 ||
        z > SC_CHUNK_RESOLUTION || z < 0)
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

    if (x > SC_CHUNK_RESOLUTION || x < 0 ||
        y > SC_CHUNK_RESOLUTION || y < 0 ||
        z > SC_CHUNK_RESOLUTION || z < 0)
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
        }
        node = child;
    }

    node->block = block;
    return 1;
}

static void
walk_chunk(sc_world_t *world, const sc_chunk_node_t **children,
           const sc_block_t *block, int x, int y, int z, size_t size,
           sc_chunk_walk_cb cb, void *closure)
{
    int i;

    if (!cb(world, size == 1 ? block : NULL, x, y, z, size,
            closure) && (size /= 2) < 1)
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
                       BLOCK_SIZE * z - BLOCK_SIZE / 2,
                       BLOCK_SIZE * y - BLOCK_SIZE / 2);
    sc_vec3_set(&vec2, BLOCK_SIZE * size, BLOCK_SIZE * size, BLOCK_SIZE * size);
    sc_vec3_add(&vec2, &vec2, &vec1);
    return sc_frustum_test_aabb(frustum, &vec1, &vec2) >= 0;
}

static int
touches_air(sc_world_t *world, int x, int y, int z)
{
    return (
        sc_world_get_block(world, x - 1, y, z)->type == SC_BLOCK_AIR ||
        sc_world_get_block(world, x, y - 1, z)->type == SC_BLOCK_AIR ||
        sc_world_get_block(world, x, y, z - 1)->type == SC_BLOCK_AIR ||
        sc_world_get_block(world, x + 1, y, z)->type == SC_BLOCK_AIR ||
        sc_world_get_block(world, x, y + 1, z)->type == SC_BLOCK_AIR ||
        sc_world_get_block(world, x, y, z + 1)->type == SC_BLOCK_AIR
    ); 
}

static int
draw_if_visible(sc_world_t *world, const sc_block_t *block, int x, int y,
                int z, size_t size, void *closure)
{
    const sc_frustum_t *frustum = closure;
    if (!contents_visible(frustum, x, y, z, size))
        return 0;
    if (size == 1 && block->texture && touches_air(world, x, y, z)) {
        glPushMatrix();
            sc_bind_texture(block->texture);
            glTranslatef(BLOCK_SIZE * x, BLOCK_SIZE * z, BLOCK_SIZE * y);
            sc_vbo_draw(block->vbo);
        glPopMatrix();
    }
    return 1;
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
    sc_free(node);
}
