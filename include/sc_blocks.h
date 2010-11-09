/* the block types for easy access */
#ifndef _INC_SC_BLOCK_H_
#define _INC_SC_BLOCK_H_

#include "sc_boot.h"
#include "sc_vbo.h"
#include "sc_texture.h"

#define SC_BLOCK_AIR            0
#define SC_BLOCK_GRASS0         1
#define SC_BLOCK_GRASS1         2
#define SC_BLOCK_GRASS2         3
#define SC_BLOCK_GRASS3         4
#define SC_BLOCK_DEADGRASS0     5
#define SC_BLOCK_DEADGRASS1     6
#define SC_BLOCK_COBBLESTONE0   7
#define SC_BLOCK_COBBLESTONE1   8
#define SC_BLOCK_COBBLESTONE2   9
#define SC_BLOCK_CLAY           10

typedef char sc_blocktype_t;

typedef struct {
    sc_texture_t *texture;
    sc_blocktype_t type;
    int falls_down;
    float movement_factor;
} sc_block_t;

#define SC_BLOCK_SLOTS 11

/* initializes the blocks.  If that does not work, a critical error is
   set and the application aborts. */
void sc_init_blocks(void);
void sc_free_blocks(void);
const char *sc_get_block_name(const sc_block_t *block);

/* returns the atlas texture for the blocks which contains all the other
   textures. */
const sc_texture_t *sc_blocks_get_atlas_texture(void);

const sc_block_t *sc_get_block(sc_blocktype_t type);
#define sc_block_falls_down(type) (sc_get_block(type)->falls_down)
#define sc_block_is_passable(type) (sc_get_block(type)->movement_factor > 0.0f)

#endif
