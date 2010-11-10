#include "sc_rnd.h"
#include <time.h>

/* the mask that is xored to the seed for the low state of the rnd */
#define HIGH_SEED_MASK 0x49616E42U

static sc_fast_rnd_t default_rnd;

void
sc_fast_rnd_seed(sc_fast_rnd_t *rnd, uint32_t seed)
{
    rnd->high = seed;
    rnd->low = seed ^ HIGH_SEED_MASK;
}

void
sc_fast_rnd_seed_time(sc_fast_rnd_t *rnd)
{
    time_t t;
    time(&t);
    sc_fast_rnd_seed(rnd, (uint32_t)t);
}

uint32_t
sc_fast_rnd_next(sc_fast_rnd_t *rnd)
{
    rnd->high = (rnd->high << 16) + (rnd->high >> 16);
    rnd->high += rnd->low;
    rnd->low += rnd->high;
    return rnd->high;
}

size_t
sc_fast_rnd_next_index(sc_fast_rnd_t *rnd, size_t max)
{
    return (size_t)(sc_fast_rnd_next(rnd) / (SC_FAST_RND_MAX / max + 1));
}

void
sc_init_default_rnd(void)
{
    sc_fast_rnd_seed_time(&default_rnd);
}

uint32_t
sc_rnd_next(void)
{
    return sc_fast_rnd_next(&default_rnd);
}

size_t sc_rnd_next_index(size_t max)
{
    return sc_fast_rnd_next_index(&default_rnd, max);
}
