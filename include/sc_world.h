/* the storage type for the world.

   The world itself is an octree with a resolution of 128x128x128. */
#ifndef _INC_SC_WORLD_H_
#define _INC_SC_WORLD_H_

#include "sc_boot.h"
#include "sc_blocks.h"
#include "sc_camera.h"

#define SC_CHUNK_RESOLUTION 128 /* size of the octree */
#define SC_CHUNK_VBO_SIZE   16  /* size of the node where vbos are stored */

struct _sc_chunk_node;
typedef struct _sc_chunk_node {
    const sc_block_t *block;
    sc_vbo_t *vbo;                      /* if there is a vbo for that node */
    unsigned int version;               /* the version of the vbo */
    struct _sc_chunk_node *children[8];
} sc_chunk_node_t;

typedef struct {
    sc_chunk_node_t *root;              /* the root node of the octree */
    uint32_t seed;                      /* the seed for the world */
    unsigned int version;
} sc_world_t;

/* callbacks for chunk traversing.  For more information have a look
   at the sc_walk_chunk function. */
typedef int (*sc_chunk_walk_cb)(sc_world_t *world, const sc_block_t *block,
                                int x, int y, int z, size_t size,
                                void *closure);


/* creates a new world */
sc_world_t *sc_new_world(void);

/* frees the world from memory again.  At that point care must be taken
   that nothing references it any more. */
void sc_free_world(sc_world_t *world);

/* returns a block for a given tripple of world coodinates. */
const sc_block_t *sc_world_get_block(sc_world_t *world, int x, int y, int z);

/* Sets a block in the world.  This will automatically rediscover the
   world if the boundaries are hit.  If something outside the hard limits
   is referenced, 0 is returned, 1 otherwise. */
int sc_world_set_block(sc_world_t *world, int x, int y, int z,
                       const sc_block_t *block);

/* draws the world.  The OpenGL projection and model matrices are used
   to calculate the visbility for the block of the world.  The camera has
   to be applied beforehand:

       sc_camera_apply(cam);
       sc_world_draw(world);
   */
void sc_world_draw(sc_world_t *world);

/* returns a vbo.  If the vbo does not exist so far or is outdated, a new
   one is created. */
const sc_vbo_t *sc_world_get_vbo(sc_world_t *world, int x, int y, int z);

/* semi-internal function to create a new chunk. */
sc_chunk_node_t *sc_new_chunk_node(void);

/* semi-internal function to free a chunk. */
void sc_free_chunk_node(sc_chunk_node_t *node);

/* traverses the world.  This will execute the given callback with
   the following information:
    - the block that for a specific node if the size is 1, otherwise the
      value for a block of that octree.  This value is totally unreliable
      unless the size is 1, so don't be too clever.
    - the x coordinate
    - the y coordinate
    - the z coordinate
    - the size of the current node you're lookint at

   If the callback returns 1 it will continue to traverse to all 8 children,
   otherwise it will stop at that point. */
void sc_walk_world(sc_world_t *world, sc_chunk_walk_cb cb, void *closure);


#endif
