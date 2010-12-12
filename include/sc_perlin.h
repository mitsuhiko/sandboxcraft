/* implements a perlin simplex noise generator */
#ifndef _INC_SC_PERLIN_H_
#define _INC_SC_PERLIN_H_

#include "sc_boot.h"

typedef struct {
    unsigned char *permutation_table;
    size_t period;
} sc_perlin_t;

/* creates a new perlin noise generator with a given seed */
sc_perlin_t *sc_new_perlin(uint32_t seed);

/* frees the perlin noise generator */
void sc_free_perlin(sc_perlin_t *perlin);

/* returns sampled 2D noise in the range -1 and 1 */
float sc_perlin_noise2(const sc_perlin_t *perlin, float x, float y);

/* returns sampled 3D noise in the range -1 and 1 */
float sc_perlin_noise3(const sc_perlin_t *perlin, float x, float y, float z);

/* extended version of noise2 with octaves */
float sc_perlin_noise2_ex(const sc_perlin_t *perlin, float x, float y,
                          int octaves);

/* extended version of noise3 with octaves */
float sc_perlin_noise3_ex(const sc_perlin_t *perlin, float x, float y,
                          float z, int octaves);

#endif
