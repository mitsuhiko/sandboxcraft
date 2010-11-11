#include "sc_boot.h"

/* internal version of the world struct.  The public one is completely
   empty as we do not expose anything currently.  This will change once
   we introduce other information there */
struct chunk_node;
typedef struct {
    uint32_t size;                      /* the size is public */
    struct chunk_node *root;            /* the root node of the octree */
} sc_world_t;

#include <assert.h>
#include <stdlib.h>

#define _SC_DONT_DEFINE_WORLD
#include "sc_engine.h"
#include "sc_blocks.h"
#include "sc_world.h"
#include "sc_frustum.h"
#include "sc_primitives.h"
#include "sc_math.h"
#include "sc_world.h"
#include "sc_perlin.h"

/* current set of flags to keep different struct sizes apart */
#define CHUNK_FLAG_LEAF         1       /* this node has no children */
#define CHUNK_FLAG_VBO          2       /* this node stores a vbo */
#define CHUNK_FLAG_DIRTY        4       /* this node's vbo needs update */

/* defines the different structs for the octree nodes.  The flags above
   tell which version of the struct we're dealing with. */
#define CHUNK_NODE_BASE char flags
#define CHUNK_NODE_CHILDREN CHUNK_NODE_BASE; struct chunk_node *children[8]
struct chunk_node { CHUNK_NODE_BASE; };
struct chunk_node_leaf { CHUNK_NODE_BASE; sc_blocktype_t block; };
struct chunk_node_children { CHUNK_NODE_CHILDREN; };
struct chunk_node_vbo { CHUNK_NODE_CHILDREN; sc_vbo_t *vbo; };

/* various helper macros to test for bits */
#define CHUNK_IS_LEAF(C)    (((C)->flags & CHUNK_FLAG_LEAF) != 0)
#define CHUNK_IS_DIRTY(C)   (((C)->flags & CHUNK_FLAG_DIRTY) != 0)
#define CHUNK_HAS_VBO(C)    (((C)->flags & CHUNK_FLAG_VBO) != 0)

/* returns the block type for a node if that node contains block information
   or SC_BLOCK_AIR if it does not carry any block info */
#define CHUNK_GET_BLOCK_TYPE(C) \
    (CHUNK_IS_LEAF(C) ? ((struct chunk_node_leaf *)C)->block : SC_BLOCK_AIR)
#define CHUNK_GET_BLOCK(C) sc_get_block(CHUNK_GET_BLOCK_TYPE(C))

/* helper check to see if any of the arguments is outside of the world */
#define OUT_OF_BOUNDS(World, X, Y, Z) \
    ((x) >= (World)->size || (x) < 0 || \
     (y) >= (World)->size || (y) < 0 || \
     (z) >= (World)->size || (z) < 0)

/* returns a 3 bit address for the given block coords */
#define CHUNK_ADDR(X, Y, Z, Size) \
    (!!((X) & (Size)) << 2 | !!((Y) & (Size)) << 1 | !!((Z) & (Size)))


static struct chunk_node *
new_chunk_node(size_t size)
{
    struct chunk_node *rv;
    if (size == 1) {
        rv = sc_xalloc(struct chunk_node_leaf);
        rv->flags = CHUNK_FLAG_LEAF;
        return rv;
    }
    if (size == SC_CHUNK_VBO_SIZE) {
        rv = sc_xalloc(struct chunk_node_vbo);
        rv->flags = CHUNK_FLAG_VBO | CHUNK_FLAG_DIRTY;
        ((struct chunk_node_vbo *)rv)->vbo = NULL;
    }
    else {
        rv = sc_xalloc(struct chunk_node_children);
        rv->flags = 0;
    }
    memset(((struct chunk_node_children *)rv)->children, 0,
           sizeof(struct chunk_node *) * 8);
    return rv;
}

static void
free_chunk_node(struct chunk_node *node)
{
    size_t i;
    if (!node)
        return;
    if (!CHUNK_IS_LEAF(node)) {
        struct chunk_node_children *cn = (struct chunk_node_children *)node;
        for (i = 0; i < 8; i++)
            free_chunk_node(cn->children[i]);
    }
    if (CHUNK_HAS_VBO(node))
        sc_free_vbo(((struct chunk_node_vbo *)node)->vbo);
    sc_free(node);
}

static struct chunk_node *
find_node(sc_world_t *world, int x, int y, int z, size_t limit)
{
    int size;
    struct chunk_node *node, *child;
    struct chunk_node_children *nodec;

    if (OUT_OF_BOUNDS(world, x, y, z))
        return NULL;

    node = world->root;
    size = world->size;
    while (1) {
        size /= 2;
        if (CHUNK_IS_LEAF(node))
            break;
        nodec = (struct chunk_node_children *)node;
        child = nodec->children[CHUNK_ADDR(x, y, z, size)];
        if (!child)
            break;
        node = child;

        if (size == limit)
            break;
    }

    return node;
}

static const sc_block_t *
get_block(sc_world_t *world, int x, int y, int z)
{
    struct chunk_node *node = find_node(world, x, y, z, 1);
    /* out of bounds or all air so far */
    if (node == NULL || !CHUNK_IS_LEAF(node))
        return sc_get_block(SC_BLOCK_AIR);
    return CHUNK_GET_BLOCK(node);
}

static struct chunk_node_vbo *
find_vbo_node(sc_world_t *world, int x, int y, int z)
{
    struct chunk_node *node = find_node(world, x, y, z, SC_CHUNK_VBO_SIZE);
    /* out of bounds or all air and so no vbo yet */
    if (!node || !CHUNK_HAS_VBO(node))
        return NULL;
    return (struct chunk_node_vbo *)node;
}

static int
set_block(sc_world_t *world, int x, int y, int z, const sc_block_t *block)
{
    int size;
    struct chunk_node *node, *child;
    struct chunk_node_children *cnode;

    if (OUT_OF_BOUNDS(world, x, y, z))
        return 0;
    else if (get_block(world, x, y, z)->type == block->type)
        return 1;

    /* Find the block in the octree */
    node = world->root;
    size = world->size;
    while (size != 1) {
        size_t idx;
        size /= 2;
        idx = CHUNK_ADDR(x, y, z, size);
        assert(!CHUNK_IS_LEAF(node));
        cnode = (struct chunk_node_children *)node;
        child = cnode->children[idx];
        if (!child) {
            child = new_chunk_node(size);
            cnode->children[idx] = child;
        }
        if (size == SC_CHUNK_VBO_SIZE)
            child->flags |= CHUNK_FLAG_DIRTY;
        node = child;
    }

    /* helper macro to mark other vbos as dirty.

       TODO: one could speed this up by remembering the parents for each
             node.  This however would require 4 additional bytes for
             each node, so it has some memory limitations.  If this ever
             becomes a performance problem, we know where to fix it. */
#define MARK_DIRTY(X, Y, Z) do { \
    struct chunk_node_vbo *node = find_vbo_node(world, X, Y, Z); \
    if (node) \
        node->flags |= CHUNK_FLAG_DIRTY; \
} while (0)

    /* in case we replace air for non air at an edge block we have to be
       extra careful and update the dirty flags of the blocks nearby. */
    if ((CHUNK_GET_BLOCK_TYPE(node) == SC_BLOCK_AIR) !=
        (block->type == SC_BLOCK_AIR)) {
        if ((x + 1) % world->size == 0) MARK_DIRTY(x + 1, y, z);
        if ((x - 1) % world->size == 0) MARK_DIRTY(x - 1, y, z);
        if ((y + 1) % world->size == 0) MARK_DIRTY(x, y + 1, z);
        if ((y - 1) % world->size == 0) MARK_DIRTY(x, y - 1, z);
        if ((z + 1) % world->size == 0) MARK_DIRTY(x, y, z + z);
        if ((z - 1) % world->size == 0) MARK_DIRTY(x, y, z - 1);
    }

    assert(CHUNK_IS_LEAF(node));
    ((struct chunk_node_leaf *)node)->block = block->type;
    return 1;
}

static void
walk_chunk(sc_world_t *world, const struct chunk_node **children,
           sc_blocktype_t block, int x, int y, int z, size_t size,
           sc_chunk_walk_cb cb, void *closure)
{
    int i;
    const struct chunk_node *child, **inner_children;

    if (!cb(world, sc_get_block(block), x, z, y, size, closure) ||
        (size /= 2) < 1)
        return;

    for (i = 0; i < 8; i++) {
        child = children ? children[i] : NULL;
        inner_children = (child && !CHUNK_IS_LEAF(child)) ?
            (const struct chunk_node **)((
                const struct chunk_node_children *)child)->children : NULL;
        walk_chunk(world, inner_children,
                   child ? CHUNK_GET_BLOCK_TYPE(child) : SC_BLOCK_AIR,
                   x + ((i & 4) ? size : 0),
                   y + ((i & 2) ? size : 0),
                   z + ((i & 1) ? size : 0),
                   size, cb, closure);
    }
}

void
sc_walk_world(sc_world_t *world, sc_chunk_walk_cb cb, void *closure)
{
    walk_chunk(world, (const struct chunk_node **)((struct
                   chunk_node_children *)world->root)->children,
               SC_BLOCK_AIR, 0, 0, 0, world->size, cb, closure);
}

static void
update_vbo(sc_world_t *world, struct chunk_node_vbo *node, int min_x,
           int min_y, int min_z, size_t size)
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
    (get_block(world, X, Y, Z)->type == SC_BLOCK_AIR)

#define ADD_PLANE(Side) do { \
    sc_cube_add_##Side##_plane(node->vbo, SC_BLOCK_SIZE, x * SC_BLOCK_SIZE, \
                               y * SC_BLOCK_SIZE, z * SC_BLOCK_SIZE); \
    sc_vbo_update_texcoords_from_texture_range( \
        node->vbo, node->vbo->vertices - 6, node->vbo->vertices, \
        block->texture); \
} while (0)

    /* we add all sides of each cube to the vbo that touch air */
    for (z = min_z; z < max_z; z++)
        for (y = min_y; y < max_y; y++)
            for (x = min_x; x < max_x; x++) {
                block = get_block(world, x, y, z);
                if (block->type == SC_BLOCK_AIR)
                    continue;

                if (IS_AIR(x - 1, y, z)) ADD_PLANE(left);
                if (IS_AIR(x + 1, y, z)) ADD_PLANE(right);
                if (IS_AIR(x, y - 1, z)) ADD_PLANE(bottom);
                if (IS_AIR(x, y + 1, z)) ADD_PLANE(top);
                if (IS_AIR(x, y, z - 1)) ADD_PLANE(back);
                if (IS_AIR(x, y, z + 1)) ADD_PLANE(front);
            }

    node->flags &= ~CHUNK_FLAG_DIRTY;
    sc_finalize_vbo(node->vbo);
}

const sc_vbo_t *
get_vbo(sc_world_t *world, int x, int y, int z)
{
    struct chunk_node_vbo *node = find_vbo_node(world, x, y, z);
    if (!node)
        return NULL;
    if (!node->vbo || CHUNK_IS_DIRTY(node))
        update_vbo(world, node, x - x % SC_CHUNK_VBO_SIZE,
                   y - y % SC_CHUNK_VBO_SIZE, z - z % SC_CHUNK_VBO_SIZE,
                   SC_CHUNK_VBO_SIZE);
    return node->vbo;
}

static int
draw_if_visible(sc_world_t *world, const sc_block_t *block, int x, int y,
                int z, size_t size, void *closure)
{
    sc_vec3_t vec1, vec2;
    const sc_frustum_t *frustum = closure;

    /* if we are not visible, abort early and do not recurse */
    sc_vec3_set(&vec1, SC_BLOCK_SIZE * x - SC_BLOCK_SIZE / 2,
                       SC_BLOCK_SIZE * z - SC_BLOCK_SIZE / 2,
                       SC_BLOCK_SIZE * y - SC_BLOCK_SIZE / 2);
    sc_vec3_set(&vec2, SC_BLOCK_SIZE * size,
                       SC_BLOCK_SIZE * size,
                       SC_BLOCK_SIZE * size);
    sc_vec3_add(&vec2, &vec2, &vec1);
    if (sc_frustum_test_aabb(frustum, &vec1, &vec2) < 0)
        return 0;

    /* we reached the level of the octree where vbos are stored.  Draw
       that and stop recursing.  get_vbo will automatically
       update the vbo if necessary. */
    if (size == SC_CHUNK_VBO_SIZE) {
        /* get_vbo operates on flipped coordinates */
        const sc_vbo_t *vbo = get_vbo(world, x, z, y);
        /* in case a piece of world is still completely empty, we won't
           have a vbo so far.  In that case, just ignore that */
        if (vbo) {
            sc_bind_texture(sc_blocks_get_atlas_texture());
            sc_vbo_draw(vbo);
        }
        return 0;
    }

    return 1;
}

static int
flush_vbos(sc_world_t *world, const sc_block_t *block, int x, int y, int z,
           size_t size, void *closure)
{
    if (size != SC_CHUNK_VBO_SIZE)
        return 1;
    get_vbo(world, x, z, y);
    return 0;
}

sc_world_t *
sc_new_world(uint32_t size)
{
    sc_world_t *world = sc_xalloc(sc_world_t);
    world->root = new_chunk_node(SC_CHUNK_VBO_SIZE);
    world->size = size;
    assert(sc_is_power_of_two(world->size));
    assert(world->size % SC_CHUNK_VBO_SIZE == 0);
    return world;
}

void
sc_free_world(sc_world_t *world)
{
    if (!world)
        return;
    free_chunk_node(world->root);
}

const sc_block_t *
sc_world_get_block(sc_world_t *world, int x, int y, int z)
{
    return get_block(world, x, z, y);
}

int
sc_world_set_block(sc_world_t *world, int x, int y, int z,
                   const sc_block_t *block)
{
    return set_block(world, x, z, y, block);
}

void
sc_world_draw(sc_world_t *world)
{
    sc_frustum_t frustum;
    sc_get_current_frustum(&frustum);
    sc_walk_world(world, draw_if_visible, &frustum);
}

void 
sc_world_flush_vbos(sc_world_t *world)
{
    sc_walk_world(world, flush_vbos, NULL);
}
