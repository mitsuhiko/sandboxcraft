#include "sc_worldgen.h"
#include "sc_rnd.h"
#include "sc_math.h"

#define DEFAULT_WATER_LEVEL     0.0f

sc_world_t *
sc_create_random_world(size_t world_size)
{
    uint32_t seed = sc_rnd_next();
    sc_worldgen_t *worldgen = sc_new_worldgen(world_size, seed);
    sc_world_t *rv = sc_worldgen_new_world(worldgen);
    sc_free_worldgen(worldgen);
    return rv;
}

sc_worldgen_t *
sc_new_worldgen(size_t world_size, uint32_t seed)
{
    sc_worldgen_t *rv = sc_xalloc(sc_worldgen_t);
    sc_fast_rnd_t rnd;
    sc_fast_rnd_seed(&rnd, seed);
    rv->world_size = world_size;
    rv->perlin = sc_new_perlin(seed);
    rv->water_level = (int)(world_size * DEFAULT_WATER_LEVEL);
    rv->off_x = sc_fast_rnd_next_float(&rnd) * 5000.0f - 2500.0f;
    rv->off_y = sc_fast_rnd_next_float(&rnd) * 5000.0f - 2500.0f;
    rv->off_z = sc_fast_rnd_next_float(&rnd) * 5000.0f - 2500.0f;
    return rv;
}

static float
positioned_base_noise(const sc_worldgen_t *worldgen, int x, int y, int z)
{
    float stretch = 0.25f * worldgen->world_size;
    return sc_perlin_noise3(worldgen->perlin,
        (x / stretch) + worldgen->off_x,
        (y / stretch) + worldgen->off_y,
        (z / stretch) + worldgen->off_z
    ) / 2.0f + 0.5f;
}

static float
positioned_overlay_noise(const sc_worldgen_t *worldgen, int x, int y, int z)
{
    float stretch = 0.05f * worldgen->world_size;
    return sc_perlin_noise3_ex(worldgen->perlin,
        (x / stretch) + worldgen->off_x * 1.5f,
        (y / stretch) + worldgen->off_y * 1.5f,
        (z / stretch) + worldgen->off_z * 1.5f,
        3, 0.5f, 1.0f, 1.0f, 2.0f
    );
}

static int
block_is_solid(const sc_worldgen_t *worldgen, int x, int y, int z)
{
    float noise, overlay;
    if (x < 0 || x >= worldgen->world_size ||
        y < 0 || y >= worldgen->world_size ||
        z < 0 || z >= worldgen->world_size)
        return 0;
    noise = positioned_base_noise(worldgen, x, y, z);
    overlay = positioned_overlay_noise(worldgen, x, y, z);
    noise += overlay * 0.1f;
    return noise > 0.9f;
}

void
sc_free_worldgen(sc_worldgen_t *worldgen)
{
    if (!worldgen)
        return;
    sc_free_perlin(worldgen->perlin);
    sc_free(worldgen);
}

sc_world_t *
sc_worldgen_new_world(const sc_worldgen_t *worldgen)
{
    int x, y, z, is_solid;
    sc_blocktype_t block;
    sc_world_t *world = sc_new_world(worldgen->world_size);
    world->water_level = 0;

    for (x = 0; x < worldgen->world_size; x++)
        for (y = 0; y < worldgen->world_size; y++)
            for (z = 0; z < worldgen->world_size; z++) {
                is_solid = block_is_solid(worldgen, x, y, z);
                if (is_solid) {
                    if (!block_is_solid(worldgen, x, y, z + 1))
                        block = SC_BLOCK_GRASS;
                    else
                        block = SC_BLOCK_DIRT;
                }
                else
                    block = SC_BLOCK_AIR;
                sc_world_set_block_fast(world, x, y, z, block);
            }

    return world;
}
