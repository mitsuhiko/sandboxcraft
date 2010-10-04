#include <assert.h>

#include "sc_blocks.h"

#define ADD_BLOCK(btype, filename, bfalls_down, bmovement_factor) do { \
    sc_block_t *block = &blocks[btype]; \
    block->texture = sc_texture_from_resource(filename, GL_NEAREST); \
    if (!block->texture) sc_error_make_critical(); \
    block->type = btype; \
    block->falls_down = bfalls_down; \
    block->movement_factor = bmovement_factor; \
} while (0)

static sc_block_t* blocks;

void
sc_init_blocks(void)
{
    if (blocks)
        return;
    blocks = sc_memassert(sc_xcalloc(SC_BLOCK_SLOTS, sizeof(sc_block_t)));

    ADD_BLOCK(SC_BLOCK_GRASS, "grass.png", 0, 0.0f);
    ADD_BLOCK(SC_BLOCK_STONE, "stone.png", 0, 0.0f);
    ADD_BLOCK(SC_BLOCK_PLANKS, "planks.png", 0, 0.0f);
    ADD_BLOCK(SC_BLOCK_SAND, "sand.png", 1, 0.0f);
    ADD_BLOCK(SC_BLOCK_GRAVEL, "gravel.png", 1, 0.0f);
    ADD_BLOCK(SC_BLOCK_COBBELSTONE, "cobblestone.png", 0, 0.0f);
    ADD_BLOCK(SC_BLOCK_WATER, "water.png", 0, 0.7f);
}

void
sc_free_blocks(void)
{
    int i;
    if (!blocks)
        return;
    for (i = 0; i < SC_BLOCK_SLOTS; i++)
        if (blocks[i].texture)
            sc_free_texture(blocks[i].texture);
    sc_free(blocks);
    blocks = 0;
}

sc_block_t *
sc_get_block(sc_blocktype_t type)
{
    assert(blocks);
    return &blocks[type];
}
