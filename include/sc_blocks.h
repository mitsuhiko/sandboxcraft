/* the block types for easy access */
#ifndef _INC_SC_BLOCK_H_
#define _INC_SC_BLOCK_H_

#include "sc_boot.h"
#include "sc_texture.h"

typedef enum {
    SC_BLOCK_GRASS = 0,
    SC_BLOCK_STONE = 1,
    SC_BLOCK_PLANKS = 2
} sc_blocktype_t;

typedef struct {
    sc_texture_t *texture;
    sc_blocktype_t type;
    int falls_down;
    float movement_factor;
} sc_block_t;

#define SC_BLOCK_SLOTS 3

void sc_init_blocks(void);
void sc_free_blocks(void);

sc_block_t *sc_get_block(sc_blocktype_t type);
#define sc_get_block_texture(type) (sc_get_block(type)->texture)
#define sc_block_falls_down(type) (sc_get_block(type)->falls_down)
#define sc_block_is_passable(type) (sc_get_block(type)->movement_factor > 0.0f)

#endif
