#include "sc_worldgen.h"
#include "sc_rnd.h"
#include "sc_math.h"

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
    rv->off_x = sc_fast_rnd_next_float(&rnd) * 5000.0f - 2500.0f;
    rv->off_y = sc_fast_rnd_next_float(&rnd) * 5000.0f - 2500.0f;
    rv->off_z = sc_fast_rnd_next_float(&rnd) * 5000.0f - 2500.0f;
    rv->off_x = rv->off_y = rv->off_z = 0.0f;
    rv->max_depth = 0.2f;
    rv->max_altitude = 0.5f;
    rv->edge_factor = 0.15f;
    rv->octaves = 3;

    return rv;
}

static int
in_base_radius(const sc_worldgen_t *worldgen, int x, int y)
{
    float r = worldgen->world_size * 0.45f;
    float edge;
    x = (x - worldgen->world_size / 2);
    y = (y - worldgen->world_size / 2);
    edge = (sc_perlin_noise2(worldgen->perlin,
        (x / (float)worldgen->world_size) * 5.0f,
        (y / (float)worldgen->world_size) * 5.0f
    ) * 0.5f + 0.5f) * pow(worldgen->edge_factor * worldgen->world_size, 2.0f);
    return (x * x + y * y) <= (r * r) - edge;
}

static void
find_extremes(const sc_worldgen_t *worldgen, int x, int y,
              int *min_out, int *max_out)
{
    float min_noise, max_noise, cutoff;
    float r = worldgen->world_size * 0.3f;
    int hx = (x - worldgen->world_size / 2);
    int hy = (y - worldgen->world_size / 2);
    max_noise = sc_perlin_noise2_ex(worldgen->perlin,
        (x / (float)worldgen->world_size) + worldgen->off_x,
        (y / (float)worldgen->world_size) + worldgen->off_y,
        worldgen->octaves, 0.7f, 1.0f, 1.0f, 2.0f) * 0.5f + 0.5f;
    min_noise = (sc_perlin_noise2(worldgen->perlin,
        (x / (float)worldgen->world_size) + worldgen->off_x * 2.2f,
        (y / (float)worldgen->world_size) + worldgen->off_y * 2.2f) * 0.5f + 0.5f);

    cutoff = (r * r) / (hx * hx + hy * hy);
    cutoff = sc_clamp(cutoff, 0.0f, 1.0f) * 0.5f;

    *min_out = (int)(min_noise * worldgen->max_depth * cutoff *
                     worldgen->world_size);
    *max_out = (int)(max_noise * worldgen->max_altitude * cutoff *
                     worldgen->world_size);
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
    int x, y, z, in_radius, min, max;
    sc_blocktype_t block;
    sc_world_t *world = sc_new_world(worldgen->world_size);

    for (x = 0; x < worldgen->world_size; x++)
        for (y = 0; y < worldgen->world_size; y++) {
            in_radius = in_base_radius(worldgen, x, y);

            /* this block is completely outside of the radius of the island.
               Just fill everything here with air from bottom to top */
            if (!in_radius) {
                for (z = 0; z < worldgen->world_size; z++)
                    sc_world_set_block_fast(world, x, y, z, SC_BLOCK_AIR);
                continue;
            }

            /* otherwise find high and down of island and set accordingly */
            find_extremes(worldgen, x, y, &min, &max);
            for (z = 0; z < worldgen->world_size; z++) {
                if (z >= min && z <= max)
                    block = z == max ? SC_BLOCK_GRASS : SC_BLOCK_DIRT;
                else
                    block = SC_BLOCK_AIR;
                sc_world_set_block_fast(world, x, y, z, block);
            }
        }

    return world;
}
