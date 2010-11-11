#ifndef _INC_SC_WORLDGEN_H_
#define _INC_SC_WORLDGEN_H_

#include "sc_world.h"
#include "sc_perlin.h"

typedef struct {
    sc_perlin_t *perlin;
    float stretch;
    float water_level;
    float beach_thresholds[2];
    float max_elevation;
    float off_x;
    float off_y;
} sc_worldgen_t;

/* generates a random new world */
sc_world_t *sc_create_random_world(size_t size);

/* creates a new world generator */
sc_worldgen_t *sc_new_worldgen(uint32_t seed);

/* frees a world generator */
void sc_free_worldgen(sc_worldgen_t *worldgen);

/* asks the world generator to fill the given world */
void sc_worldgen_fill_world(const sc_worldgen_t *worldgen, sc_world_t *world);

#endif
