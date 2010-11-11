#include <assert.h>

#include "sc_blocks.h"
#include "sc_atlas.h"
#include "sc_primitives.h"

#define ADD_BLOCK(Type, Filename, FallsDown, MovementFactor) do { \
    sc_block_t *block = &blocks[Type]; \
    block->texture = sc_atlas_add_from_resource(block_atlas, Filename); \
    if (!block->texture) sc_error_make_critical(); \
    block->type = Type; \
    block->falls_down = FallsDown; \
    block->movement_factor = MovementFactor; \
} while (0)

static sc_block_t *blocks;
static sc_atlas_t *block_atlas;

void
sc_init_blocks(void)
{
    if (blocks)
        return;
    blocks = sc_memassert(sc_xcalloc(SC_BLOCK_SLOTS, sizeof(sc_block_t)));
    block_atlas = sc_new_atlas(128, 128, GL_NEAREST);

    /* air is special, it's not really a block.  This works because the ID
       of the air block is zero. */
    memset(&blocks[SC_BLOCK_AIR], 0, sizeof(sc_block_t));
    assert(SC_BLOCK_AIR == 0);

    /* an indestructable foundation block */
    ADD_BLOCK(SC_BLOCK_BEDROCK, "bedrock.png", 0, 0.0f);

    /* regular blocks */
    ADD_BLOCK(SC_BLOCK_GRASS, "grass.png", 0, 0.0f);
    ADD_BLOCK(SC_BLOCK_STONE, "stone.png", 0, 0.0f);
    ADD_BLOCK(SC_BLOCK_WATER, "water.png", 0, 0.4f);
    ADD_BLOCK(SC_BLOCK_SAND, "sand.png", 0, 0.0f);
    ADD_BLOCK(SC_BLOCK_DIRT, "dirt.png", 1, 0.0f);

    if (!sc_finalize_atlas(block_atlas))
        sc_error_make_critical();
}

const char *
sc_get_block_name(const sc_block_t *block)
{
    switch (block->type) {
    case SC_BLOCK_AIR:          return "Air";
    case SC_BLOCK_BEDROCK:      return "Bedrock";
    case SC_BLOCK_WATER:        return "Water";
    case SC_BLOCK_GRASS:        return "Grass";
    case SC_BLOCK_STONE:        return "Stone";
    case SC_BLOCK_SAND:         return "Sand";
    case SC_BLOCK_DIRT:         return "Dirt";
    default:                    return "Unknown";
    }
}

void
sc_free_blocks(void)
{
    if (!blocks)
        return;
    sc_free_atlas(block_atlas);
    sc_free(blocks);
    blocks = 0;
}

const sc_block_t *
sc_get_block(sc_blocktype_t type)
{
    assert(blocks);
    return &blocks[(int)type];
}

const sc_texture_t *
sc_blocks_get_atlas_texture(void)
{
    return sc_atlas_get_texture(block_atlas);
}
