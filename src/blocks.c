#include <assert.h>

#include "sc_blocks.h"
#include "sc_atlas.h"
#include "sc_primitives.h"

#define ADD_BLOCK(btype, filename, bfalls_down, bmovement_factor) do { \
    sc_block_t *block = &blocks[btype]; \
    block->texture = sc_atlas_add_from_resource(block_atlas, filename); \
    block->vbo = make_cube_vbo(block->texture); \
    if (!block->texture) sc_error_make_critical(); \
    block->type = btype; \
    block->falls_down = bfalls_down; \
    block->movement_factor = bmovement_factor; \
} while (0)

static sc_block_t *blocks;
static sc_atlas_t *block_atlas;

static sc_vbo_t *
make_cube_vbo(const sc_texture_t *texture)
{
    sc_vbo_t *vbo = sc_new_cube(20.0f);
    //sc_finalize_vbo(vbo);
    return vbo;
}

void
sc_init_blocks(void)
{
    if (blocks)
        return;
    blocks = sc_memassert(sc_xcalloc(SC_BLOCK_SLOTS, sizeof(sc_block_t)));
    block_atlas = sc_new_atlas(64, 64, GL_NEAREST);

    ADD_BLOCK(SC_BLOCK_GRASS, "grass.png", 0, 0.0f);
    ADD_BLOCK(SC_BLOCK_STONE, "stone.png", 0, 0.0f);
    ADD_BLOCK(SC_BLOCK_PLANKS, "planks.png", 0, 0.0f);
    ADD_BLOCK(SC_BLOCK_SAND, "sand.png", 1, 0.0f);
    ADD_BLOCK(SC_BLOCK_GRAVEL, "gravel.png", 1, 0.0f);
    ADD_BLOCK(SC_BLOCK_COBBLESTONE, "cobblestone.png", 0, 0.0f);
    ADD_BLOCK(SC_BLOCK_WATER, "water.png", 0, 0.7f);

    if (!sc_finalize_atlas(block_atlas))
        sc_error_make_critical();
}

const char *
sc_get_block_name(sc_blocktype_t type)
{
    switch (type) {
    case SC_BLOCK_GRASS:        return "Grass";
    case SC_BLOCK_STONE:        return "Stone";
    case SC_BLOCK_PLANKS:       return "Planks";
    case SC_BLOCK_SAND:         return "Sand";
    case SC_BLOCK_GRAVEL:       return "Gravel";
    case SC_BLOCK_COBBLESTONE:  return "Cobblestone";
    case SC_BLOCK_WATER:        return "Water";
    default:                    return "Unknown";
    }
}

void
sc_free_blocks(void)
{
    int i;
    if (!blocks)
        return;
    sc_free(block_atlas);

    for (i = 0; i < SC_BLOCK_SLOTS; i++)
        sc_free_vbo(blocks[i].vbo);

    sc_free(blocks);
    blocks = 0;
}

sc_block_t *
sc_get_block(sc_blocktype_t type)
{
    assert(blocks);
    return &blocks[type];
}
