#include <assert.h>

#include "sc_blocks.h"

#define ADD_BLOCK(btype, filename, bfalls_down, bmovement_factor) do { \
    sc_block_t *block = &blocks[btype]; \
    block->texture = sc_texture_from_resource(filename, GL_NEAREST); \
    block->type = btype; \
    block->falls_down = bfalls_down; \
    block->movement_factor = bmovement_factor; \
} while (0)

sc_block_t* blocks;

void
sc_init_blocks(void)
{
    if (blocks)
        return;

    blocks = sc_memassert(sc_xmalloc(sizeof(sc_block_t) * SC_BLOCK_SLOTS));

    ADD_BLOCK(SC_BLOCK_GRASS, "grass.png", 0, 0.0f);
    ADD_BLOCK(SC_BLOCK_STONE, "stone.png", 0, 0.0f);
}

void
sc_free_blocks(void)
{
    int i;
    if (!blocks)
        return;
    for (i = 0; i < SC_BLOCK_SLOTS; i++)
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
