#ifndef _INC_SC_WORLDGEN_H_
#define _INC_SC_WORLDGEN_H_

#include "sc_world.h"
#include "sc_perlin.h"

typedef struct {
    sc_perlin_t *perlin;
    size_t world_size;
    float off_x;
    float off_y;
    float off_z;
    float max_altitude;
    float max_depth;
    float edge_factor;
    int octaves;
} sc_worldgen_t;

/* generates a random new world */
sc_world_t *sc_create_random_world();

/* creates a new world generator */
sc_worldgen_t *sc_new_worldgen(size_t world_size, uint32_t seed);

/* frees a world generator */
void sc_free_worldgen(sc_worldgen_t *worldgen);

/* asks the world generator to fill the given world */
sc_world_t *sc_worldgen_new_world(const sc_worldgen_t *worldgen);

#endif
