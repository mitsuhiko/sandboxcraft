#include "sc_boot.h"
#include "zlib.h"

/* internal version of the world struct.  The public one is completely
   empty as we do not expose anything currently.  This will change once
   we introduce other information there */
struct chunk_node;
typedef struct {
    size_t size;                        /* the size is public */
    int state;                          /* the state the world is in */
    struct chunk_node *root;            /* the root node of the octree */
} sc_world_t;

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
#include "sc_list.h"
#include "sc_utils.h"

/* current set of flags to keep different struct sizes apart */
#define CHUNK_FLAG_LEAF         1       /* this node has no children */
#define CHUNK_FLAG_VBO          2       /* this node stores a vbo */
#define CHUNK_FLAG_DIRTY        4       /* this node's vbo needs update */

/* different states of the world */
#define WORLD_STATE_EARLY       1       /* not yet finalized */
#define WORLD_STATE_PREFILLED   2       /* filled once, disables fast setting */
#define WORLD_STATE_FINALIZED   3       /* light updates happen on change */

/* defines the different structs for the octree nodes.  The flags above
   tell which version of the struct we're dealing with. */
#define CHUNK_NODE_BASE uint8_t flags; sc_blocktype_t block
#define CHUNK_NODE_CHILDREN CHUNK_NODE_BASE; struct chunk_node *children[8]
struct chunk_node { CHUNK_NODE_BASE; };
struct chunk_node_children { CHUNK_NODE_CHILDREN; };
struct chunk_node_vbo { CHUNK_NODE_CHILDREN; sc_vbo_t *vbo; uint8_t *light; };

/* various helper macros to test for bits */
#define CHUNK_IS_LEAF(C)    (((C)->flags & CHUNK_FLAG_LEAF) != 0)
#define CHUNK_IS_DIRTY(C)   (((C)->flags & CHUNK_FLAG_DIRTY) != 0)
#define CHUNK_HAS_VBO(C)    (((C)->flags & CHUNK_FLAG_VBO) != 0)

/* returns the block type for a node if that node contains block information
   or SC_BLOCK_AIR if it does not carry any block info */
#define CHUNK_GET_BLOCK(C) (((struct chunk_node *)C)->block)

/* helper check to see if any of the arguments is outside of the world */
#define OUT_OF_BOUNDS(World, X, Y, Z) \
    ((x) >= (World)->size || (x) < 0 || \
     (y) >= (World)->size || (y) < 0 || \
     (z) >= (World)->size || (z) < 0)

/* helper that creates an aabb for a given block */
#define MAKE_BLOCK_AABB(V1, V2, X, Y, Z, Size) do { \
    sc_vec3_set(V1, SC_BLOCK_SIZE * X - SC_BLOCK_SIZE / 2, \
                    SC_BLOCK_SIZE * Y - SC_BLOCK_SIZE / 2, \
                    SC_BLOCK_SIZE * Z - SC_BLOCK_SIZE / 2); \
    sc_vec3_set(V2, SC_BLOCK_SIZE * Size, \
                    SC_BLOCK_SIZE * Size, \
                    SC_BLOCK_SIZE * Size); \
    sc_vec3_add(V2, V2, V1); \
} while (0)

/* returns a 3 bit address for the given block coords */
#define CHUNK_ADDR(X, Y, Z, Size) \
    (!!((X) & (Size)) << 2 | !!((Y) & (Size)) << 1 | !!((Z) & (Size)))

/* size of the light container */
#define LIGHT_CONTAINER_SIZE (SC_CHUNK_VBO_SIZE * SC_CHUNK_VBO_SIZE * \
                              SC_CHUNK_VBO_SIZE)

/* returns the address of a block's light information in a vbo node's
   light info array.  The coordinates can be global as they are
   transposed into the node local coordinate system automatically */
#define LIGHT_ADDR(X, Y, Z) \
    (((X) % SC_CHUNK_VBO_SIZE) + \
     ((Y) % SC_CHUNK_VBO_SIZE) * SC_CHUNK_VBO_SIZE + \
     ((Z) % SC_CHUNK_VBO_SIZE) * SC_CHUNK_VBO_SIZE * SC_CHUNK_VBO_SIZE)

/* maximum size of the free lists for chunks */
#define FREELIST_SIZE 64

/* maximum depth of an octree below the chunk size.  This has to be at
   least log(SC_CHUNK_VBO_SIZE) large. */
#define MAX_RELATIONS 32

/* memory to keep for draw information */
#define VBOINFO_STACK_SIZE (1 << 16)

/* free lists for leaf and children nodes which are frequently allocated
   and deallocated. */
static struct chunk_node *free_leaf_nodes[FREELIST_SIZE];
static struct chunk_node *free_children_nodes[FREELIST_SIZE];
static size_t free_leaf_nodes_count;
static size_t free_children_nodes_count;

/* vbo infos are also often created and removed (up to ~5000 times a frame)
   and because of that are managed by a stack allocator. */

struct vboinfo {
    const sc_vbo_t *vbo;
    float distance;
};

/* the closure passed to the draw functions */
struct chunk_draw_closure {
    const sc_frustum_t *frustum;
    const sc_camera_t *camera;
    sc_list_t *vbos;
    sc_stackalloc_t *stackalloc;
};


static struct chunk_node *
new_leaf_chunk_node(sc_blocktype_t block_type)
{
    struct chunk_node *rv;
    if (free_leaf_nodes_count)
        rv = free_leaf_nodes[--free_leaf_nodes_count];
    else
        rv = sc_xalloc(struct chunk_node);
    rv->flags = CHUNK_FLAG_LEAF;
    rv->block = block_type;
    return rv;
}

static struct chunk_node *
new_chunk_node_with_children(sc_blocktype_t block_type, int with_vbo)
{
    struct chunk_node_children *rv;
    if (with_vbo) {
        uint8_t *light_info = sc_xmalloc(LIGHT_CONTAINER_SIZE);
        rv = sc_xalloc(struct chunk_node_vbo);
        rv->flags = CHUNK_FLAG_VBO | CHUNK_FLAG_DIRTY;
        ((struct chunk_node_vbo *)rv)->vbo = NULL;
        ((struct chunk_node_vbo *)rv)->light = light_info;
    } else {
        if (free_children_nodes_count)
            rv = (struct chunk_node_children *)
                free_children_nodes[--free_children_nodes_count];
        else
            rv = sc_xalloc(struct chunk_node_children);
        rv->flags = 0;
    }
    rv->block = block_type;
    memset(rv->children, 0, sizeof(struct chunk_node *) * 8);
    return (struct chunk_node *)rv;
}

static void
free_chunk_node(struct chunk_node *node)
{
    size_t i;
    if (!node)
        return;

    /* helper macro that attempts to add the given node to one of the
       two free lists.  If it succeeds it will return from the function
       without further code execution */
#define TRY_PUT_ON_FREELIST(List, Node) do { \
    if (free_##List##_nodes_count < FREELIST_SIZE) { \
        free_##List##_nodes[free_##List##_nodes_count++] = node; \
        return; \
    } \
} while (0)

    if (CHUNK_IS_LEAF(node)) {
        TRY_PUT_ON_FREELIST(leaf, node);
    } else {
        struct chunk_node_children *cn;
        if (!CHUNK_HAS_VBO(node)) {
            TRY_PUT_ON_FREELIST(children, node);
        } else {
            sc_free_vbo(((struct chunk_node_vbo *)node)->vbo);
            sc_free(((struct chunk_node_vbo *)node)->light);
        }
        cn = (struct chunk_node_children *)node;
        for (i = 0; i < 8; i++)
            free_chunk_node(cn->children[i]);
    }
    sc_free(node);
}

static struct chunk_node *
find_node(sc_world_t *world, int x, int y, int z, size_t limit,
          struct chunk_node *node, size_t size, size_t *size_out)
{
    struct chunk_node *child;
    struct chunk_node_children *nodec;

    if (!node) {
        node = world->root;
        size = world->size;
    }

    if (OUT_OF_BOUNDS(world, x, y, z))
        return NULL;

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

    if (size_out)
        *size_out = size;
    return node;
}

static sc_blocktype_t
get_block_below(sc_world_t *world, int x, int y, int z,
                struct chunk_node *node, size_t size)
{
    struct chunk_node *rv = find_node(world, x, y, z, 1, node, size, NULL);
    return (rv == NULL) ? SC_BLOCK_AIR : CHUNK_GET_BLOCK(rv);
}

static struct chunk_node_vbo *
find_vbo_node(sc_world_t *world, int x, int y, int z,
              struct chunk_node *node, size_t size, size_t *size_out)
{
    struct chunk_node *rv = find_node(world, x, y, z, SC_CHUNK_VBO_SIZE,
                                      node, size, size_out);
    /* out of bounds or all air and so no vbo yet */
    if (!rv || !CHUNK_HAS_VBO(rv))
        return NULL;
    return (struct chunk_node_vbo *)rv;
}

static int
any_children_set(const struct chunk_node_children *node)
{
    size_t i;
    for (i = 0; i < 8; i++)
        if (node->children[i])
            return 1;
    return 0;
}

static int
set_native_lighting(sc_world_t *world, sc_blocktype_t block, int x, int y,
                    int z, size_t size, void *closure)
{
    /* not yet native, but 1.0 looks nicer :) */
    sc_world_set_block_light(world, x, y, z, 1.0f);
    return 1;
}

static void
calculate_lighting(sc_world_t *world)
{
    sc_walk_world(world, set_native_lighting, NULL);
}

static void
calculate_incremental_lighting(sc_world_t *world, int x, int y, int z)
{
}

static float
block_light_from_vbo_node(struct chunk_node_vbo *node, int x, int y, int z)
{
    size_t offset;
    offset = LIGHT_ADDR(x, y, z);
    assert(offset < LIGHT_CONTAINER_SIZE);
    return *(node->light + offset) / 255.0f;
}

static void
compress_partial_tree(struct chunk_node **relations, size_t count)
{
    struct chunk_node *node;
    struct chunk_node_children *cnode, *parent;
    size_t i, j, k, found_block;
    sc_blocktype_t block;

    for (i = count - 1; i > 0; i--) {
        node = relations[i];

        if (CHUNK_IS_LEAF(node))
            continue;
        cnode = (struct chunk_node_children *)node;

        /* check the simple case where all children are not yet set */
        if (!any_children_set(cnode)) {
            block = cnode->block;

        /* the case where we completely ignore the node's block value
           and check if all nodes have the same type */
        } else {
            found_block = 0;
            for (j = 0; j < 8; j++) {
                sc_blocktype_t actual_block = !cnode->children[j]
                    ? cnode->block : cnode->children[j]->block;
                if (!found_block) {
                    block = actual_block;
                    found_block = 1;
                } else if (actual_block != block) {
                    return;
                }
            }
        }

        parent = (struct chunk_node_children *)relations[i - 1];
        assert(!CHUNK_IS_LEAF(parent));

        for (k = 0; k < 8; k++)
            if (parent->children[k] == node)
                break;
        assert(k < 8);
        parent->children[k] = new_leaf_chunk_node(block);
        free_chunk_node(node);
    }
}

static int
set_block(sc_world_t *world, int x, int y, int z, sc_blocktype_t block,
          int early_optimizations)
{
    size_t size, idx, last_relation = 0;
    struct chunk_node *node, *child;
    struct chunk_node_children *cnode;
    struct chunk_node *relations[MAX_RELATIONS];
    struct chunk_node *vbo_container = 0;

    /* that won't work in non early states */
    if (early_optimizations)
        assert(world->state == WORLD_STATE_EARLY);

    /* bail out early */
    if (OUT_OF_BOUNDS(world, x, y, z)) {
        return 0;
    } else if (early_optimizations) {
        if (block == SC_BLOCK_AIR)
            return 1;
    } else if (sc_world_get_block(world, x, y, z) == block) {
        return 1;
    }

    /* Find the block in the octree and modify it */
    node = world->root;
    size = world->size;
    while (1) {
        size /= 2;

        /* remember the vbo containing node */
        if (size == SC_CHUNK_VBO_SIZE)
            vbo_container = node;

        /* things below the vbo size may be optimized into a single leaf
           node if necessary.  This saves some memory if we repeat the
           same pattern all over again */
        else if (size < SC_CHUNK_VBO_SIZE)
            relations[last_relation++] = node;

        assert(!CHUNK_IS_LEAF(node));
        cnode = (struct chunk_node_children *)node;
        idx = CHUNK_ADDR(x, y, z, size);
        child = cnode->children[idx];

        /* at size 1 we just set the child and that's pretty much it */
        if (size == 1) {
            if (!child) {
                child = new_leaf_chunk_node(block);
                cnode->children[idx] = child;
            } else {
                child->block = block;
            }
            relations[last_relation++] = child;
            break;
        }

        /* this chunk is a leaf.  That's unfortunate because we have to
           further recurse to set this child.  Split up that node and
           prepare the children for value setting. */
        if (!child || CHUNK_IS_LEAF(child)) {
            int is_vbo = size == SC_CHUNK_VBO_SIZE;
            struct chunk_node *old_child = child;
            child = new_chunk_node_with_children(old_child
                ? old_child->block : node->block, is_vbo);
            free_chunk_node(old_child);
            cnode->children[idx] = child;
        }

        if (CHUNK_HAS_VBO(child))
            child->flags |= CHUNK_FLAG_DIRTY;
        node = child;
    }

    /* at that point we have the information stored, but we can attempt
       to compress it.  This will collapse child nodes to leaf nodes
       if all children have the same information stored. */
    compress_partial_tree(relations, last_relation);

    /* if we assume the whole world is already dirty we can skip this */
    if (early_optimizations)
        return 1;

    /* make sure we have found a container */
    assert(vbo_container);

    /* helper macro to mark other vbos as dirty.  This attempts to find
       the vbo in the container first, and if that does not work will
       try from toplevel again.  If a node does not exist yet it will
       try again from toplevel and fail there too.  This is currently
       intentional as for a fully filled world that will not happen
       that often.  Even nodes of air will have an (albeit empty) vbo
       attached. */
#define MARK_DIRTY(X, Y, Z) do { \
    struct chunk_node_vbo *rv = find_vbo_node(world, X, Y, Z, vbo_container, \
                                              SC_CHUNK_VBO_SIZE * 2, NULL); \
    if (!rv) \
        rv = find_vbo_node(world, X, Y, Z, NULL, 0, NULL); \
    if (rv) \
        rv->flags |= CHUNK_FLAG_DIRTY; \
} while (0)

    /* in case we replace air for non air at an edge block we have to be
       extra careful and update the dirty flags of the blocks nearby. */
    if ((CHUNK_GET_BLOCK(node) == SC_BLOCK_AIR) != (block == SC_BLOCK_AIR)) {
        if ((x + 1) % world->size == 0) MARK_DIRTY(x + 1, y, z);
        if ((x - 1) % world->size == 0) MARK_DIRTY(x - 1, y, z);
        if ((y + 1) % world->size == 0) MARK_DIRTY(x, y + 1, z);
        if ((y - 1) % world->size == 0) MARK_DIRTY(x, y - 1, z);
        if ((z + 1) % world->size == 0) MARK_DIRTY(x, y, z + z);
        if ((z - 1) % world->size == 0) MARK_DIRTY(x, y, z - 1);
    }

    /* automatically upgrade the state now if necessary */
    if (world->state == WORLD_STATE_EARLY)
        world->state = WORLD_STATE_PREFILLED;
    /* in finalized state we have to upgrade the lighting now */
    else if (world->state == WORLD_STATE_FINALIZED)
        calculate_incremental_lighting(world, x, y, z);

    return 1;
}

static void
walk_chunk(sc_world_t *world, const struct chunk_node **children,
           sc_blocktype_t block, int x, int y, int z, size_t size,
           sc_chunk_walk_cb cb, void *closure)
{
    int i;
    const struct chunk_node *child, **inner_children;

    if (!cb(world, block, x, y, z, size, closure) ||
        (size /= 2) < 1)
        return;

    for (i = 0; i < 8; i++) {
        child = children ? children[i] : NULL;
        inner_children = (child && !CHUNK_IS_LEAF(child)) ?
            (const struct chunk_node **)((
                const struct chunk_node_children *)child)->children : NULL;
        walk_chunk(world, inner_children,
                   child ? CHUNK_GET_BLOCK(child) : SC_BLOCK_AIR,
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
    sc_blocktype_t block;
    int x, y, z;
    int max_x = min_x + size;
    int max_y = min_y + size;
    int max_z = min_z + size;
    int vbo_flags = SC_VBO_LIGHT | SC_VBO_DYNAMIC | SC_VBO_3D_TEXTURE;

    if (node->vbo)
        sc_vbo_reuse(node->vbo, vbo_flags);
    else
        node->vbo = sc_new_vbo(vbo_flags);

    /* quick test if a neighboring node is air.  This will attempt to check
       the current vbo's child nodes if possible and if we have to deal with
       a neighbouring vbo, we go up the whole tree as we do not know the
       parent of the vbo */
#define IS_AIR(X, Y, Z) \
    ((((X) >= min_x && (X) < (max_x) && \
       (Y) >= min_y && (Y) < (max_y) && \
       (Z) >= min_z && (Z) < (max_z)) \
         ? get_block_below(world, X, Y, Z, (struct chunk_node *)node, size) \
         : sc_world_get_block(world, X, Y, Z)) == SC_BLOCK_AIR)

    /* calls into the primitive cube functions to add a given side of a
       cube to the vbo with the texture of the block as well as the light
       information stored. */
#define ADD_PLANE(Side, TextureIndex) do { \
    sc_cube_add_##Side##_plane(node->vbo, SC_BLOCK_SIZE, x * SC_BLOCK_SIZE, \
                               y * SC_BLOCK_SIZE, z * SC_BLOCK_SIZE); \
    sc_vbo_update_texcoords_range( \
        node->vbo, node->vbo->vertices - 6, node->vbo->vertices, \
        sc_get_block(block)->textures[TextureIndex]); \
    sc_vbo_update_light_range( \
        node->vbo, node->vbo->vertices - 6, node->vbo->vertices, \
        block_light_from_vbo_node(node, x, y, z)); \
} while (0)

    /* we add all sides of each cube to the vbo that touch air */
    for (z = min_z; z < max_z; z++)
        for (y = min_y; y < max_y; y++)
            for (x = min_x; x < max_x; x++) {
                block = get_block_below(world, x, y, z,
                                        (struct chunk_node *)node, size);
                if (block == SC_BLOCK_AIR)
                    continue;

                if (IS_AIR(x - 1, y, z)) ADD_PLANE(left, SC_LEFT_BLOCK_SIDE);
                if (IS_AIR(x + 1, y, z)) ADD_PLANE(right, SC_RIGHT_BLOCK_SIDE);
                if (IS_AIR(x, y - 1, z)) ADD_PLANE(bottom, SC_BOTTOM_BLOCK_SIDE);
                if (IS_AIR(x, y + 1, z)) ADD_PLANE(top, SC_TOP_BLOCK_SIDE);
                if (IS_AIR(x, y, z - 1)) ADD_PLANE(far, SC_FAR_BLOCK_SIDE);
                if (IS_AIR(x, y, z + 1)) ADD_PLANE(near, SC_NEAR_BLOCK_SIDE);
            }

    node->flags &= ~CHUNK_FLAG_DIRTY;
    sc_vbo_finalize(node->vbo);
}

const sc_vbo_t *
get_vbo(sc_world_t *world, int x, int y, int z)
{
    struct chunk_node_vbo *node = find_vbo_node(world, x, y, z, NULL, 0, NULL);
    if (!node)
        return NULL;
    if (!node->vbo || CHUNK_IS_DIRTY(node))
        update_vbo(world, node, x - x % SC_CHUNK_VBO_SIZE,
                   y - y % SC_CHUNK_VBO_SIZE, z - z % SC_CHUNK_VBO_SIZE,
                   SC_CHUNK_VBO_SIZE);
    return node->vbo;
}

static int
find_visible_vbos(sc_world_t *world, sc_blocktype_t block, int x, int y,
                  int z, size_t size, void *closure)
{
    sc_vec3_t vec1, vec2, distance;
    struct chunk_draw_closure *args = closure;

    /* if we are not visible, abort early and do not recurse */
    MAKE_BLOCK_AABB(&vec1, &vec2, x, y, z, size);
    if (sc_frustum_test_aabb(args->frustum, &vec1, &vec2) < 0)
        return 0;

    /* we reached the level of the octree where vbos are stored.  Draw
       that and stop recursing.  get_vbo will automatically
       update the vbo if necessary. */
    if (size == SC_CHUNK_VBO_SIZE) {
        const sc_vbo_t *vbo = get_vbo(world, x, y, z);
        /* in case a piece of world is still completely empty, we won't
           have a vbo so far.  In that case, just ignore that.  We also
           ignore a generated vbo that has not a single vertex */
        if (vbo && vbo->vertices > 0) {
            struct vboinfo *info = sc_stackalloc_alloc(args->stackalloc,
                                                       sizeof(struct vboinfo));
            /* run out of memory on the stack, try again with heap.  We
               make sure that this heap allocated memory is removed at the
               end of the rendering */
            if (!info)
                info = sc_xalloc(struct vboinfo);
            sc_vec3_sub(&distance, &vec1, &args->camera->position);
            info->vbo = vbo;
            info->distance = sc_vec3_length2(&distance);
            sc_list_append(args->vbos, info);
        }
        return 0;
    }

    return 1;
}

static int
flush_vbos(sc_world_t *world, sc_blocktype_t block, int x, int y, int z,
           size_t size, void *closure)
{
    if (size != SC_CHUNK_VBO_SIZE)
        return 1;
    get_vbo(world, x, y, z);
    return 0;
}

sc_world_t *
sc_new_world(uint32_t size)
{
    sc_world_t *world = sc_xalloc(sc_world_t);
    world->root = new_chunk_node_with_children(SC_BLOCK_AIR, 0);
    world->size = size;
    world->state = WORLD_STATE_EARLY;
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

sc_blocktype_t
sc_world_get_block(sc_world_t *world, int x, int y, int z)
{
    struct chunk_node *rv = find_node(world, x, y, z, 1, NULL, 0, NULL);
    return (rv == NULL) ? SC_BLOCK_AIR : CHUNK_GET_BLOCK(rv);
}

float
sc_world_get_block_light(sc_world_t *world, int x, int y, int z)
{
    static struct chunk_node_vbo *vbo_node;
    vbo_node = find_vbo_node(world, x, y, z, NULL, 0, NULL);
    if (!vbo_node)
        return 0.0f;
    return block_light_from_vbo_node(vbo_node, x, y, z);
}

int
sc_world_set_block_light(sc_world_t *world, int x, int y, int z, float val)
{
    size_t offset;
    static struct chunk_node_vbo *vbo_node;
    vbo_node = find_vbo_node(world, x, y, z, NULL, 0, NULL);
    if (!vbo_node)
        return 0;
    offset = LIGHT_ADDR(x, y, z);
    assert(offset < LIGHT_CONTAINER_SIZE);
    *(vbo_node->light + offset) = (uint8_t)(val * 255.0f);
    return 1;
}

int
sc_world_set_block(sc_world_t *world, int x, int y, int z,
                   sc_blocktype_t block)
{
    return set_block(world, x, y, z, block, 0);
}

int
sc_world_set_block_fast(sc_world_t *world, int x, int y, int z,
                        sc_blocktype_t block)
{
    return set_block(world, x, y, z, block, 1);
}

static int
compare_vbo_by_distance(const void *v1, const void *v2, void *closure)
{
    struct vboinfo *a = *(struct vboinfo **)v1;
    struct vboinfo *b = *(struct vboinfo **)v2;
    return sc_cmp(a->distance, b->distance);
}

void
sc_world_draw(sc_world_t *world, const sc_camera_t *cam)
{
    size_t i;
    sc_frustum_t frustum;
    struct chunk_draw_closure closure;
    char vboinfo_stack[VBOINFO_STACK_SIZE];
    sc_stackalloc_t vboinfo_stackalloc;

    assert(world->state == WORLD_STATE_FINALIZED);

    sc_stackalloc_init(&vboinfo_stackalloc, &vboinfo_stack,
                       VBOINFO_STACK_SIZE);
    sc_get_current_frustum(&frustum);
    closure.frustum = &frustum;
    closure.camera = cam;
    closure.vbos = sc_new_list();
    closure.stackalloc = &vboinfo_stackalloc;

    sc_walk_world(world, find_visible_vbos, &closure);
    sc_list_sort(closure.vbos, compare_vbo_by_distance, NULL);

    if (closure.vbos->size > 0) {
        sc_texture_bind(sc_blocks_get_combined_texture());
        for (i = 0; i < closure.vbos->size; i++) {
            struct vboinfo *info = closure.vbos->items[i];
            sc_vbo_draw(info->vbo);

            /* if the pointer was not on the stack it has to be on the heap
               so we have to free it here before we leave the function */
            if (!sc_stackalloc_managed(&vboinfo_stackalloc, info))
                sc_free(info);
        }
    }

    sc_free_list(closure.vbos);
}

void 
sc_world_flush_vbos(sc_world_t *world)
{
    sc_walk_world(world, flush_vbos, NULL);
}

void
sc_world_finalize(sc_world_t *world)
{
    assert(world->state != WORLD_STATE_FINALIZED);
    world->state = WORLD_STATE_FINALIZED;
    calculate_lighting(world);
}

int
sc_world_save(sc_world_t *world, const char *filename)
{
    int x, y, z;
    gzFile file;

    /* current file format requires blocks being a char */
    assert(sizeof(sc_blocktype_t) == 1);

    file = gzopen(filename, "wb");
    if (!file) {
        gzclose(file);
        sc_set_error(SC_EIO, filename, 0, "Could not save the world.");
        return 0;
    }

    /* version and world size */
    gzputc(file, '\x01');
    gzputc(file, (world->size >> 8) & 0xff);
    gzputc(file, world->size & 0xff);

    for (x = 0; x < world->size; x++)
        for (y = 0; y < world->size; y++)
            for (z = 0; z < world->size; z++)
                gzputc(file, (int)sc_world_get_block(world, x, y, z));

    gzclose(file);
    return 1;
}

sc_world_t *
sc_world_load(const char *filename)
{
    char buffer[4];
    int x, y, z;
    int size;
    char version;
    sc_world_t *rv;
    gzFile file = gzopen(filename, "rb");

    if (!file) {
        sc_set_error(SC_ENOENT, filename, 0, "World save not found.");
        return NULL;
    }

    version = gzgetc(file);
    if (version != 1) {
        gzclose(file);
        sc_set_error(SC_EIO, filename, 0, "Invalid world version");
        return NULL;
    }

    gzread(file, buffer, 2);
    size = (int)buffer[0] << 8 | (int)buffer[1];

    if (size > 512 || size < 16 || !sc_is_power_of_two(size)) {
        gzclose(file);
        sc_set_error(SC_EIO, filename, 0, "Invalid world size (%d)", size);
        return NULL;
    }

    rv = sc_new_world(size);

    for (x = 0; x < size; x++)
        for (y = 0; y < size; y++)
            for (z = 0; z < size; z++) {
                int block = gzgetc(file);
                sc_world_set_block(rv, x, y, z, (sc_blocktype_t)block);
            }

    gzclose(file);
    sc_world_finalize(rv);

    return rv;
}
