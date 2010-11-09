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

/* returns a new random integer */
size_t sc_fast_rnd_next_index(sc_fast_rnd_t *rnd, size_t max);

#endif
