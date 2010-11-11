#include "sc_worldgen.h"
#include "sc_rnd.h"

#define DEFAULT_STRETCH         0.6f
#define DEFAULT_WATER_LEVEL     0.2f
#define DEFAULT_LOW_BEACH       0.015f
#define DEFAULT_HIGH_BEACH      0.01f
#define DEFAULT_MAX_ELEVATION   0.4f

#define REG_UNDERWATER      0
#define REG_BEACH           1
#define REG_MEADOW          2

sc_world_t *
sc_create_random_world(size_t world_size)
{
    uint32_t seed = sc_rnd_next();
    sc_worldgen_t *worldgen = sc_new_worldgen(seed);
    sc_world_t *rv = sc_new_world(world_size);
    sc_worldgen_fill_world(worldgen, rv);
    sc_free_worldgen(worldgen);
    return rv;
}

sc_worldgen_t *
sc_new_worldgen(uint32_t seed)
{
    sc_worldgen_t *rv = sc_xalloc(sc_worldgen_t);
    sc_fast_rnd_t rnd;
    sc_fast_rnd_seed(&rnd, seed);
    rv->perlin = sc_new_perlin(seed);
    rv->stretch = DEFAULT_STRETCH;
    rv->water_level = DEFAULT_WATER_LEVEL;
    rv->beach_thresholds[0] = DEFAULT_LOW_BEACH;
    rv->beach_thresholds[1] = DEFAULT_HIGH_BEACH;
    rv->max_elevation = DEFAULT_MAX_ELEVATION;
    rv->off_x = sc_fast_rnd_next_float(&rnd) * 5000.0f - 2500.0f;
    rv->off_y = sc_fast_rnd_next_float(&rnd) * 5000.0f - 2500.0f;
    return rv;
}

void
sc_free_worldgen(sc_worldgen_t *worldgen)
{
    if (!worldgen)
        return;
    sc_free_perlin(worldgen->perlin);
    sc_free(worldgen);
}

static size_t
get_height(const sc_worldgen_t *worldgen, int x, int y, size_t world_size)
{
    size_t max_height = (size_t)(world_size * worldgen->max_elevation);
    float stretch = worldgen->stretch * world_size;
    float noise = sc_perlin_noise2(worldgen->perlin,
        (x / stretch) + worldgen->off_x,
        (y / stretch) + worldgen->off_y
    ) / 2.0f + 0.5f;
    size_t height = (size_t)(noise * max_height);
    return height <= 1 ? 1 : height;
}

void
sc_worldgen_fill_world(const sc_worldgen_t *worldgen, sc_world_t *world)
{
    int x, y, z, region;
    sc_blocktype_t block;
    size_t h;
    size_t water_level = (size_t)(worldgen->water_level * world->size);
    size_t beach_l = (size_t)(water_level -
                              worldgen->beach_thresholds[0] * world->size);
    size_t beach_h = (size_t)(water_level +
                              worldgen->beach_thresholds[1] * world->size);

    for (x = 0; x < world->size; x++)
        for (y = 0; y < world->size; y++) {
            h = get_height(worldgen, x, y, world->size);
            if (h >= beach_l && h <= beach_h)
                region = REG_BEACH;
            else if (h < water_level)
                region = REG_UNDERWATER;
            else
                region = REG_MEADOW;

            for (z = 0; z < world->size; z++) {
                block = SC_BLOCK_AIR;
                if (z <= 1)
                    block = SC_BLOCK_BEDROCK;
                else if (region == REG_UNDERWATER) {
                    if (z < h)
                        block = SC_BLOCK_STONE;
                }
                else if (region == REG_BEACH) {
                    if (z < h - 3)
                        block = SC_BLOCK_STONE;
                    else if (h - 3 <= z && z <= h)
                        block = SC_BLOCK_SAND;
                }
                else if (region == REG_MEADOW) {
                    if (z < h - 4)
                        block = SC_BLOCK_STONE;
                    else if (h - 4 <= z && z < h)
                        block = SC_BLOCK_DIRT;
                    else if (h == z)
                        block = SC_BLOCK_GRASS;
                }
                if (block == SC_BLOCK_AIR && z < water_level)
                    block = SC_BLOCK_WATER;
                sc_world_set_block(world, x, y, z, sc_get_block(block));
            }
        }
}
