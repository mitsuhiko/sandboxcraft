/* the storage type for the world.

   The idea is that the world has a maximum height (128) and is based on
   chunks of a given size (128x128).  When you run out of space in a
   horizontal direction a new chunk is added.  The chunks themselves are
   addressed by positive and negative numbers where 0, 0 is the center
   of the world.

   Each chunk is an octree by itself.

   This means that upon exploration of the world, the map moves the
   pointers to the octree roots around in the world->known array so
   that the center of the new world is again the original root chunk.
   As a second effect the world always grows symmetrically.
   
   */
#ifndef _INC_SC_WORLD_H_
#define _INC_SC_WORLD_H_

#include "sc_boot.h"
#include "sc_blocks.h"
#include "sc_camera.h"

#define SC_CHUNK_RESOLUTION     64
#define SC_CHUNK_LIMIT          1024

struct _sc_chunk_node;
typedef struct _sc_chunk_node {
    const sc_block_t *block;
    struct _sc_chunk_node *children[8];
} sc_chunk_node_t;

typedef struct {
    sc_chunk_node_t *root;
    int x;
    int y;
    int version;                        /* incremented whenever a block in
                                           the chunk is modified.  Overflows
                                           are fine */
} sc_chunk_t;

typedef struct {
    sc_chunk_t **known;                 /* the known world :) */
    uint32_t seed;                      /* the seed for the world */
    size_t width;
    size_t height;
} sc_world_t;

/* callbacks for chunk traversing.  For more information have a look
   at the sc_walk_chunk function. */
typedef int (*sc_chunk_walk_cb)(const sc_block_t *block,
                                int x, int y, int z, size_t size,
                                void *closure);


/* creates a new world */
sc_world_t *sc_new_world(void);

/* frees the world from memory again.  At that point care must be taken
   that nothing references it any more. */
void sc_free_world(sc_world_t *world);

/* can be used to automatically discover new parts of the world.  This
   can be called when the view is expanded beyond SC_CHUNK_RESOLUTION.
   The z value is currently ignored, 0 can be assumed to probe into
   the wild. */
void sc_probe_world(sc_world_t *world, int x, int y, int z);

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

/* semi-internal function to create a new chunk. */
sc_chunk_node_t *sc_new_chunk_node(void);

/* semi-internal function to free a chunk. */
void sc_free_chunk_node(sc_chunk_node_t *node);

/* traverses a given chunk.  This will execute the given callback with
   the following information:
    - the block that for a specific node if the size is 1, otherwise NULL.
    - the x coordinate
    - the y coordinate
    - the z coordinate
    - the size of the current node you're lookint at

   If the callback returns 1 it will continue to traverse to all 8 children,
   otherwise it will stop at that point. */
void sc_walk_chunk(sc_chunk_t *chunk, sc_chunk_walk_cb cb, void *closure);


#endif
