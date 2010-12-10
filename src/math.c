#include "sc_math.h"


size_t
sc_next_power_of_two(size_t value)
{
    int i;
    value -= 1;
    for (i = 0; i < sizeof(size_t) * 8; i <<= 1)
        value = value | value >> i;
    return value + 1;
}

size_t
sc_intlog2(size_t value)
{
    size_t rv = 0;
    while (value >>= 1)
        ++rv;
    return rv;
}
