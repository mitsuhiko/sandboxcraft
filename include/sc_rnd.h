/* random number generators.  The fast rnd of this module is based on the
   random generator of ian bullard.  It's used for our noise functions until
   something faster can be found that yields comparable results. */
#ifndef _INC_SC_RND_H_
#define _INC_SC_RND_H_

#include "sc_boot.h"

typedef struct {
    uint32_t low;
    uint32_t high;
} sc_fast_rnd_t;

#define SC_FAST_RND_MAX (uint32_t)(-1)

/* seeds the fast rnd from the given value */
void sc_fast_rnd_seed(sc_fast_rnd_t *rnd, uint32_t seed);

/* seeds the fast rnd from the current time */
void sc_fast_rnd_seed_time(sc_fast_rnd_t *rnd);

/* returns a new random number */
uint32_t sc_fast_rnd_next(sc_fast_rnd_t *rnd);

/* returns a new random positive integer */
size_t sc_fast_rnd_next_index(sc_fast_rnd_t *rnd, size_t max);

/* returns a random floating point number between 0.0f and 1.0f */
float sc_fast_rnd_next_float(sc_fast_rnd_t *rnd);

/* initializes the default rnd.  This happens at engine initialization time */
void sc_init_default_rnd();

/* returns a new random number from the default rnd */
uint32_t sc_rnd_next(void);

/* returns a new random positive integer from the default rnd */
size_t sc_rnd_next_index(size_t max);

/* returns a random floating point number between 0.0f and 1.0f */
float sc_rnd_next_float(void);

#endif
