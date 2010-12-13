#include "sc_blocks.h"
#include "sc_arraytex.h"
#include "sc_primitives.h"

#define LOAD_TEXTURE(Id, Fn) \
    const sc_texture_t *Id = assert_texture_loaded(\
        sc_arraytex_add_from_resource(block_arraytex, Fn))
#define DECLARE_BLOCK(Type, Top, Side, Bottom, FallsDown, MovementFactor) do { \
    sc_block_t *block = &blocks[Type]; \
    block->type = Type; \
    block->textures[SC_LEFT_BLOCK_SIDE] = Side; \
    block->textures[SC_RIGHT_BLOCK_SIDE] = Side; \
    block->textures[SC_TOP_BLOCK_SIDE] = Top; \
    block->textures[SC_BOTTOM_BLOCK_SIDE] = Bottom; \
    block->textures[SC_NEAR_BLOCK_SIDE] = Side; \
    block->textures[SC_FAR_BLOCK_SIDE] = Side; \
    block->falls_down = FallsDown; \
    block->movement_factor = MovementFactor; \
    block->emits_light = 0.0f; \
    block->translucency = 0.0f; \
} while (0)
#define DECLARE_SIMPLE_BLOCK(Type, Tex, FallsDown, MovementFactor) \
    DECLARE_BLOCK(Type, Tex, Tex, Tex, FallsDown, MovementFactor)
#define SET_LIGHT(Type, Light, Translucency) do { \
    blocks[Type].emits_light = Light; \
    blocks[Type].translucency = Translucency; \
} while (0)

static sc_block_t *blocks;
static sc_arraytex_t *block_arraytex;

static const sc_texture_t *
assert_texture_loaded(const sc_texture_t *ptr)
{
    if (!ptr) sc_error_make_critical();
    return ptr;
}

static void
create_blocks(void)
{
    /* load textures first */
    LOAD_TEXTURE(grass, "grass.png");
    LOAD_TEXTURE(grass_side, "grass-side.png");
    LOAD_TEXTURE(bedrock, "bedrock.png");
    LOAD_TEXTURE(stone, "stone.png");
    LOAD_TEXTURE(sand, "sand.png");
    LOAD_TEXTURE(water, "water.png");
    LOAD_TEXTURE(dirt, "dirt.png");

    /* the blocks */
    DECLARE_SIMPLE_BLOCK(SC_BLOCK_BEDROCK, bedrock, 0, 0.0f);
    DECLARE_BLOCK(SC_BLOCK_GRASS, grass, grass_side, dirt, 0, 0.0f);
    DECLARE_SIMPLE_BLOCK(SC_BLOCK_STONE, stone, 0, 0.0f);
    DECLARE_SIMPLE_BLOCK(SC_BLOCK_WATER, water, 0, 0.4f);
    DECLARE_SIMPLE_BLOCK(SC_BLOCK_SAND, sand, 0, 0.0f);
    DECLARE_SIMPLE_BLOCK(SC_BLOCK_DIRT, dirt, 1, 0.0f);

    /* update light information */
    SET_LIGHT(SC_BLOCK_AIR, 0, 1.0f);
}

void
sc_init_blocks(void)
{
    if (blocks)
        return;
    blocks = sc_memassert(sc_xcalloc(SC_BLOCK_SLOTS, sizeof(sc_block_t)));
    block_arraytex = sc_new_arraytex(256, 256, SC_ARRAYTEX_NEAREST |
                                               SC_ARRAYTEX_MIPMAPS);

    /* air is special, it's not really a block.  This works because the ID
       of the air block is zero. */
    memset(&blocks[SC_BLOCK_AIR], 0, sizeof(sc_block_t));
    assert(SC_BLOCK_AIR == 0);
    create_blocks();

    if (!sc_arraytex_finalize(block_arraytex))
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
    sc_free_arraytex(block_arraytex);
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
sc_blocks_get_combined_texture(void)
{
    return sc_arraytex_get_texture(block_arraytex);
}
