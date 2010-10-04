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
   As a second effect the world always grows symmetrically.  This does
   not yet mean that the world in the other direction was generated,
   it just means that space is allocated for a pointer that could point
   upon discovery to a new octree root.
   
   */
#ifndef _INC_SC_WORLD_H_
#define _INC_SC_WORLD_H_

#include "sc_boot.h"
#include "sc_blocks.h"

#define SC_CHUNK_RESOLUTION 128

struct _sc_chunk_node;
typedef struct _sc_chunk_node {
    sc_block_t *block;
    struct _sc_chunk_node *children[8];
} sc_chunk_node_t;

typedef struct {
    sc_chunk_node_t *root;
    int x;
    int y;
} sc_chunk_t;

typedef struct {
    sc_chunk_t **known;                 /* the known world :) */
    uint32_t seed;                      /* the seed for the world */
    size_t size_x;
    size_t size_y;
} sc_world_t;


sc_world_t *sc_new_world(void);
void sc_free_world(sc_world_t *world);
#define sc_world_get_chunk(world, x, y) \
    (&world->known[y + (world->size_y / 2)][x + (world->size_x / 2)])

sc_block_t *sc_world_get_block(sc_world_t *world, int x, int y, int z);
int sc_world_set_block(sc_world_t *world, int x, int y, int z, sc_block_t *block);

sc_chunk_node_t *sc_new_chunk_node(void);
void sc_free_chunk_node(sc_chunk_node_t *node);


#endif
