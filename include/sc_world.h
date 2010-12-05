/* the storage type for the world.

   The world itself is an octree with a resolution specified at world
   creation time.
   
   Extra care has to be taken on the coordinate system.  In OpenGL and
   this engine, the ground plane has the y vector as normal.  Because
   this is awkward when thinking in terms of blocks in the world, the
   world itself uses a different coordinate system.

   The normal to the ground is the z vector and the ground spans x and y.
   Internally however the whole world is based on the OpenGL coordinate
   system.  The translation between these two coordinate systems happens
   on the public API here.

   All functions that deal with coordinates have the world coordinate
   system here and will dynamically calculate the transformation as
   necessary.  This is usually fine but might be a little bit confusing
   in the actual implementation where y and z sometimes change place. */
#ifndef _INC_SC_WORLD_H_
#define _INC_SC_WORLD_H_

#include "sc_boot.h"
#include "sc_blocks.h"
#include "sc_camera.h"

/* where the vbos are stored.  chunk_resolution % vbo_size === 0! */
#define SC_CHUNK_VBO_SIZE   16

/* the size of a block in the world */
#define SC_BLOCK_SIZE 1.0f

/* the world is an opaque type */
#ifndef _SC_DONT_DEFINE_WORLD
typedef struct {
    size_t size;
    size_t water_level;
} sc_world_t;
#endif

/* callbacks for chunk traversing.  For more information have a look
   at the sc_walk_chunk function. */
typedef int (*sc_chunk_walk_cb)(sc_world_t *world, const sc_block_t *block,
                                int x, int y, int z, size_t size,
                                void *closure);

/* creates a new world */
sc_world_t *sc_new_world(uint32_t size);

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

/* sets a block without updating the dirty flag of vbos.  This might only
   be used before a single vbo was rendered and can be used to faster fill
   the initial set of the world.  You also must not override a block that
   was already set. */
int sc_world_set_block_fast(sc_world_t *world, int x, int y, int z,
                            const sc_block_t *block);

/* draws the world.  The OpenGL projection and model matrices are used
   to calculate the visbility for the block of the world.

   This currently has some internal optimizations applied so that this
   function is not reentrant.  This is usually not a problem but if it
   might be necessary in the future to call this function recursively,
   the implementation has to be adapted. */
void sc_world_draw(sc_world_t *world, const sc_camera_t *cam);

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

/* builds the vbos from the current state of the world.  When many changes
   occur in the world at once (such as when the game starts up) it makes
   sense to halt the render loop and show a loading screen and then call
   this method to update the vbos at once. */
void sc_world_flush_vbos(sc_world_t *world);

#endif
