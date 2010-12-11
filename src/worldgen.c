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
    sc_fast_rnd_seed(&rnd, 0);
    rv->world_size = world_size;
    rv->perlin = sc_new_perlin(seed);
    rv->water_level = (int)(world_size * DEFAULT_WATER_LEVEL);
    rv->off_x = sc_fast_rnd_next_float(&rnd) * 5000.0f - 2500.0f;
    rv->off_y = sc_fast_rnd_next_float(&rnd) * 5000.0f - 2500.0f;
    rv->off_z = sc_fast_rnd_next_float(&rnd) * 5000.0f - 2500.0f;
    rv->off_x = rv->off_y = rv->off_z = 0.0f;
    return rv;
}

static float
positioned_base_noise(const sc_worldgen_t *worldgen, int x, int y, int z)
{
    float radius = worldgen->world_size / 2.0f;
    float half = worldgen->world_size / 2.0f;
    float xh = x - half;
    float yh = y - half;
    float zh = z - half;
    float density = (radius * radius) /
        ((radius * radius) - (xh * xh + yh * yh + zh * zh));
    if (density < 0.0f)
        density = 0.0f;
    return sc_perlin_noise3_ex(worldgen->perlin,
        (x / (float)worldgen->world_size) + worldgen->off_x,
        (y / (float)worldgen->world_size) + worldgen->off_y,
        (z / (float)worldgen->world_size) + worldgen->off_z,
        5, 0.5f, 1.0f, 10.0f, 2.0f
    ) * pow(density, 2.0f);
}

static int
block_is_solid(const sc_worldgen_t *worldgen, int x, int y, int z)
{
    float noise;
    if (x < 0 || x >= worldgen->world_size ||
        y < 0 || y >= worldgen->world_size ||
        z < 0 || z >= worldgen->world_size)
        return 0;
    noise = positioned_base_noise(worldgen, x, y, z);
    return noise > 0.5f;
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
