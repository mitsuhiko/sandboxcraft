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
    rv->perlin = sc_new_perlin(seed);
    rv->world_size = world_size;
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

static int
block_is_solid(const sc_worldgen_t *worldgen, float x, float y, float z)
{
    float noise, density, center_falloff, plateau_falloff;
    
    /* caves */
    if (powf(sc_perlin_noise3_ex(worldgen->perlin, x * 5.0f, y * 5.0f,
                                 z * 5.0f, 3), 3.0f) < -0.5f)
        return 0;

    /* falloff from the top */
    if (y > 0.9f)
        return SC_BLOCK_AIR;
    else if (y > 0.8f)
        plateau_falloff = 1.0f - (y - 0.8f) * 10.0f;
    else
        plateau_falloff = 1.0f;

    /* falloff from center */
    center_falloff = 0.1f / (
        powf((x - 0.5f) * 1.5f, 2.0f) +
        powf((y - 1.0f) * 0.8f, 2.0f) +
        powf((z - 0.5f) * 1.5f, 2.0f)
    );

    /* noise combined density */
    noise = sc_perlin_noise3_ex(worldgen->perlin, x, y * 0.5, z, 7);
    density = noise * center_falloff * plateau_falloff;
    return density > 0.2f ? SC_BLOCK_STONE : SC_BLOCK_AIR;
}

static void
generate_rock(const sc_worldgen_t *worldgen, sc_world_t *world)
{
    int x, y, z;
    sc_blocktype_t block;

    for (x = 0; x < worldgen->world_size; x++)
        for (y = 0; y < worldgen->world_size; y++)
            for (z = 0; z < worldgen->world_size; z++) {
                block = block_is_solid(worldgen,
                    (float)x / worldgen->world_size,
                    (float)y / worldgen->world_size,
                    (float)z / worldgen->world_size
                ) ? SC_BLOCK_STONE : SC_BLOCK_AIR;
                sc_world_set_block_fast(world, x, y, z, block);
            }
}

static void
remove_solitaries(const sc_worldgen_t *worldgen, sc_world_t *world)
{
    int x, y, z;

#define IS_AIR(X, Y, Z) (sc_world_get_block(world, X, Y, Z) == SC_BLOCK_AIR)

    for (x = 0; x < worldgen->world_size; x++)
        for (y = 0; y < worldgen->world_size; y++)
            for (z = 0; z < worldgen->world_size; z++) {
                if (!IS_AIR(x, y, z))
                    continue;
                if (IS_AIR(x + 1, y, z) && IS_AIR(x - 1, y, z) &&
                    IS_AIR(x, y + 1, z) && IS_AIR(x, y - 1, z) &&
                    IS_AIR(x, y, z + 1) && IS_AIR(x, y, z - 1))
                    sc_world_set_block(world, x, y, z, SC_BLOCK_AIR);
            }
}

static void
erode_down(const sc_worldgen_t *worldgen, sc_world_t *world,
           int x, int y, int z)
{
    sc_blocktype_t block;
    int levels_left = 1 + 4 * (sc_perlin_noise3(worldgen->perlin,
        x / (float)worldgen->world_size,
        y / (float)worldgen->world_size,
        z / (float)worldgen->world_size
    ) * 0.5 + 0.5);

    for (; y > 0 && levels_left > 0; y--, levels_left--) {
        block = sc_world_get_block(world, x, y, z);
        if (block != SC_BLOCK_STONE)
            return;
        sc_world_set_block(world, x, y, z, SC_BLOCK_DIRT);
    }
}

static void
erode_world(const sc_worldgen_t *worldgen, sc_world_t *world)
{
    sc_blocktype_t block;
    int x, y, z, above_the_air;

    for (x = 0; x < worldgen->world_size; x++)
        for (z = 0; z < worldgen->world_size; z++) {
            above_the_air = 1;
            for (y = worldgen->world_size - 1; y > 0; y--) {
                block = sc_world_get_block(world, x, y, z);
                if (block == SC_BLOCK_AIR) {
                    above_the_air = 1;
                    continue;
                } else if (block == SC_BLOCK_STONE) {
                    if (above_the_air)
                        erode_down(worldgen, world, x, y, z);
                }
                above_the_air = 0;
            }
        }
}

static void
grow_grass(const sc_worldgen_t *worldgen, sc_world_t *world)
{
    int x, y, z;

    for (x = 0; x < worldgen->world_size; x++)
        for (y = 0; y < worldgen->world_size; y++)
            for (z = worldgen->world_size - 1; z > 0; z--) {
                if (sc_world_get_block(world, x, y, z) == SC_BLOCK_DIRT &&
                    sc_world_get_block(world, x, y + 1, z) == SC_BLOCK_AIR)
                    sc_world_set_block(world, x, y, z, SC_BLOCK_GRASS);
            }
}

sc_world_t *
sc_worldgen_new_world(const sc_worldgen_t *worldgen)
{
    sc_world_t *world = sc_new_world(worldgen->world_size);

    /* fill the world with one large floating rock (and maybe some small
       rocks).  This uses fast block setting and because of that must
       absolutely happen first. */
    generate_rock(worldgen, world);

    /* remove solitary blocks floating around */
    remove_solitaries(worldgen, world);

    /* next step is eroding stone into dirt */
    erode_world(worldgen, world);

    /* grow grass on the high dirt levels when they touch air */
    grow_grass(worldgen, world);

    sc_world_finalize(world);
    return world;
}
