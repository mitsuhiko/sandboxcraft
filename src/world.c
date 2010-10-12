#include <assert.h>
#include <stdlib.h>

#include "sc_world.h"
#include "sc_frustum.h"

#define BLOCK_DIMENSION 20.0f

/* helper that discovers a single chunk. x/y are world coords */
static void
discover_chunk(sc_world_t *world, sc_chunk_t *chunk, int x, int y)
{
    chunk->root = sc_new_chunk_node();
    chunk->x = x;
    chunk->y = y;
    chunk->version = 0;
    /* TODO: use a perlin noise function here to generate a map */
}

/* helper for resolve block that expands the world as necessary */
static void
discover_world(sc_world_t *world, int new_width, int new_height)
{
    int y, x, half_width, half_height;
    int old_height = world->height;
    sc_chunk_t **old_known = world->known;
    int off_x = (new_width - world->width) / 2;
    int off_y = (new_height - world->height) / 2;

    /* create the new world */
    world->known = sc_xcalloc(new_height, sizeof(sc_chunk_t *));
    world->width = new_width;
    world->height = new_height;
    half_width = new_width / 2;
    half_height = new_height / 2;

    /* fill the new world with the old contents if any and fill blanks
       with newly generated world. */
    for (y = 0; y < (int)world->height; y++) {
        sc_chunk_t *row = sc_xcalloc(new_width, sizeof(sc_chunk_t));
        world->known[y] = row;
        /* existing row */
        if (y >= off_y && y < (int)world->height - off_y - 1) {
            for (x = 0; x < (int)world->width; x++)
                if (x >= off_x && x < (int)world->width - off_x - 1) {
                    row[x].root = old_known[y][x - off_x].root;
                    /* update reflected location information */
                    row[x].x = x - half_width;
                    row[x].y = y - half_height;
                }
                else
                    discover_chunk(world, &row[x], x - half_width,
                                   y - half_height);
        }
        /* completely new row */
        else
            for (x = 0; x < (int)world->width; x++)
                discover_chunk(world, &row[x], x - half_width,
                               y - half_height);
    }

    /* free old world pointers */
    for (y = 0; y < old_height; y++)
        sc_free(old_known[y]);
    sc_free(old_known);
}

static sc_chunk_t *
resolve_block(sc_world_t *world, int x, int y, int z, int discover,
              int *local_x, int *local_y)
{
    int chunk_x, chunk_y;

    /* z is limited by the resolution of a single chunk */
    if (z < 0 || z >= SC_CHUNK_RESOLUTION)
        return NULL;

    chunk_x = (world->width / 2) + x / SC_CHUNK_RESOLUTION;
    chunk_y = (world->height / 2) + y / SC_CHUNK_RESOLUTION;

    /* tried to reference a non-explored chunk in the non-discovered
       world and we don't want to discover right now.  Bail out */
    if (!discover) {
        if (chunk_x < 0 || chunk_y < 0 ||
            chunk_x >= (int)world->width || chunk_y >= (int)world->height)
            return NULL;
    }
    /* there is also a hard limit we must not hit, make sure we're
       below that. */
    else if (chunk_x < -SC_CHUNK_LIMIT || chunk_x >= SC_CHUNK_LIMIT ||
             chunk_y < -SC_CHUNK_LIMIT || chunk_y >= SC_CHUNK_LIMIT)
        return NULL;
    /* at that point we have to make sure we expand the known world
       as necessary. */
    else if (chunk_x < 0 || chunk_y < 0 ||
             chunk_x > (int)world->width || chunk_y > (int)world->height) {
        discover_world(world, world->width + abs(chunk_x),
                       world->height + abs(chunk_y));
        /* we have to recalculate the chunk offsets because the
           world size changed */
        chunk_x = (world->width / 2) + x / SC_CHUNK_RESOLUTION;
        chunk_y = (world->height / 2) + y / SC_CHUNK_RESOLUTION;
    }

    /* if local coordinates are requested, calculate them */
    if (local_x) *local_x = x % SC_CHUNK_RESOLUTION;
    if (local_y) *local_y = y % SC_CHUNK_RESOLUTION;

    return &world->known[chunk_y][chunk_x];
}

sc_world_t *
sc_new_world(void)
{
    sc_world_t *world = sc_xalloc(sc_world_t);
    world->known = NULL;
    world->width = 0;
    world->height = 0;
    discover_world(world, 1, 1);
    return world;
}

void
sc_free_world(sc_world_t *world)
{
    size_t x, y;
    if (!world)
        return;

    for (x = 0; x < world->width; x++)
        for (y = 0; y < world->height; y++) {
            sc_chunk_t *chunk = &world->known[y][x];
            if (chunk)
                sc_free_chunk_node(chunk->root);
        }
}

void
sc_probe_world(sc_world_t *world, int x, int y, int z)
{
    resolve_block(world, x, y, z, 1, NULL, NULL);
}

sc_block_t *
sc_world_get_block(sc_world_t *world, int x, int y, int z)
{
    int local_x, local_y, size;
    sc_chunk_t *chunk;
    sc_chunk_node_t *node, *child;

    chunk = resolve_block(world, x, y, z, 0, &local_x, &local_y);
    if (!chunk)
        return NULL;

    /* locate the block in the octree */
    node = chunk->root;
    size = SC_CHUNK_RESOLUTION;
    while (size != 1) {
        size_t idx;
        size /= 2;
        idx = !!(local_x & size) << 2 | !!(local_y & size) << 1 | !!(z & size);
        child = node->children[idx];
        if (!child)
            break;
        node = child;
    }

    return node->block;
}

int
sc_world_set_block(sc_world_t *world, int x, int y, int z, sc_block_t *block)
{
    int local_x, local_y, size;
    sc_chunk_t *chunk;
    sc_chunk_node_t *node, *child;

    chunk = resolve_block(world, x, y, z, 1, &local_x, &local_y);
    /* we're past the hard boundaries or z was too low/high */
    if (!chunk)
        return 0;

    /* Find the block in the octree */
    node = chunk->root;
    size = SC_CHUNK_RESOLUTION;
    while (size != 1) {
        size_t idx;
        size /= 2;
        idx = !!(local_x & size) << 2 | !!(local_y & size) << 1 | !!(z & size);
        child = node->children[idx];
        if (!child) {
            child = sc_new_chunk_node();
            node->children[idx] = child;
        }
        node = child;
    }

    node->block = block;
    chunk->version++;
    return 1;
}

void
sc_world_draw(sc_world_t *world)
{
    int x, y, z;
    sc_frustum_t frustum;
    sc_vec3_t block_dimensions;

    sc_get_current_frustum(&frustum);
    sc_vec3_set(&block_dimensions, BLOCK_DIMENSION,
                BLOCK_DIMENSION, BLOCK_DIMENSION);

    for (z = 0; z < SC_CHUNK_RESOLUTION; z++)
        for (y = 0; y < SC_CHUNK_RESOLUTION; y++)
            for (x = 0; x < SC_CHUNK_RESOLUTION; x++) {
                sc_block_t *block = sc_world_get_block(world, x, y, z);
                sc_vec3_t vec1, vec2;
                sc_vec3_set(&vec1, BLOCK_DIMENSION * x,
                                   BLOCK_DIMENSION * z,
                                   BLOCK_DIMENSION * y);
                sc_vec3_add(&vec2, &vec1, &block_dimensions);
                if (!block || sc_frustum_test_aabb(&frustum, &vec1, &vec2) < 0)
                    continue;
                glPushMatrix();
                    sc_bind_texture(block->texture);
                    glTranslatef(20.0f * x, 20.0f * z, 20.0f * y);
                    sc_vbo_draw(block->vbo);
                glPopMatrix();
            }
}

sc_chunk_node_t *
sc_new_chunk_node(void)
{
    sc_chunk_node_t *node = sc_xalloc(sc_chunk_node_t);
    memset(node->children, 0, sizeof(node->children));
    node->block = NULL;
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
