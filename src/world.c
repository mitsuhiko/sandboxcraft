#include <assert.h>

#include "sc_world.h"

#define DEFAULT_BLOCK SC_BLOCK_STONE


/* discover a new chunk that is a neighbour of ref.  Requires that the
   world was already resized to hold the given chunk. */
static sc_chunk_t *
discover_chunk(sc_world_t *world, sc_chunk_t *ref, int offx, int offy)
{
    int x = (ref ? ref->x : 0) + offx;
    int y = (ref ? ref->y : 0) + offy;
    sc_chunk_t *chunk = sc_world_get_chunk(world, x, y);
    chunk->x = x;
    chunk->y = y;
    chunk->root = sc_new_chunk_node();
    return chunk;
}

static sc_chunk_t *
resolve_block(sc_world_t *world, int x, int y, int z, int *local_x, int *local_y)
{
    int half_x, half_y, chunk_x, chunk_y;

    /* z is limited by the resolution of a single chunk */
    if (z < 0 || z >= SC_CHUNK_RESOLUTION)
        return NULL;

    half_x = world->size_x / 2;
    half_y = world->size_y / 2;
    chunk_x = half_x + x / SC_CHUNK_RESOLUTION;
    chunk_y = half_y + y / SC_CHUNK_RESOLUTION;

    /* tried to reference a non-explored chunk in the world */
    if (chunk_x < 0 || chunk_y < 0 || chunk_x > half_x || chunk_y > half_y)
        return NULL;

    /* there is a chunk, return it and get local coordinates */
    *local_x = x % SC_CHUNK_RESOLUTION;
    *local_y = y % SC_CHUNK_RESOLUTION;
    return sc_world_get_chunk(world, chunk_x, chunk_y);
}

sc_world_t *
sc_new_world(void)
{
    sc_world_t *world = sc_xalloc(sc_world_t);
    world->known = sc_xalloc(sc_chunk_t *);
    world->known[0] = sc_xalloc(sc_chunk_t);
    world->size_x = 1;
    world->size_y = 1;
    discover_chunk(world, NULL, 0, 0);
    return world;
}

void
sc_free_world(sc_world_t *world)
{
    size_t x, y;

    for (x = 0; x < world->size_x; x++)
        for (y = 0; y < world->size_y; y++) {
            sc_chunk_t *chunk = &world->known[y][x];
            if (chunk)
                sc_free_chunk_node(chunk->root);
        }
}

sc_block_t *
sc_world_get_block(sc_world_t *world, int x, int y, int z)
{
    int local_x, local_y, size;
    sc_chunk_t *chunk;
    sc_chunk_node_t *node, *child;

    chunk = resolve_block(world, x, y, z, &local_x, &local_y);
    if (!chunk)
        return NULL;

    /* locate the block in the octree */
    node = chunk->root;
    size = SC_CHUNK_RESOLUTION;
    while (size) {
        if (node->is_leaf)
            return node->block;
        size /= 2;
        child = node->children[(local_x & size) << 2 |
                               (local_y & size) << 1 |
                               (z & size)];
        /* missing child means use block of parent */
        if (!child)
            return node->block;
        node = child;
    }

    /* this should not happen */
    assert(0);
    return NULL;
}

int
sc_world_set_block(sc_world_t *world, int x, int y, int z, sc_block_t *block)
{
    int local_x, local_y, size;
    sc_chunk_t *chunk;
    sc_chunk_node_t **node;

    chunk = resolve_block(world, x, y, z, &local_x, &local_y);
    /* blocks can only be set in the known world */
    if (!chunk)
        return 0;

    /* Find the block in the octree */
    node = &chunk->root;
    size = SC_CHUNK_RESOLUTION;
    while (size) {
        if (!*node)
            *node = sc_new_chunk_node();
        else if (!(**node).is_leaf) {
            size /= 2;
            node = &(**node).children[(local_x & size) << 2 |
                                      (local_y & size) << 1 |
                                      (z & size)];
        }
        else
            break;
    }

    (**node).block = block;
    return 1;
}

sc_chunk_node_t *
sc_new_chunk_node(void)
{
    sc_chunk_node_t *node = sc_xalloc(sc_chunk_node_t);
    memset(node->children, 0, sizeof(node->children));
    node->block = sc_get_block(DEFAULT_BLOCK);
    node->is_leaf = 1;
    return node;
}

void
sc_free_chunk_node(sc_chunk_node_t *node)
{
    if (!node)
        return;
    size_t i;
    for (i = 0; i < 8; i++)
        sc_free_chunk_node(node->children[i]);
    sc_free(node);
}
