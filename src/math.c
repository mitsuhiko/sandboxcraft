#include "sc_math.h"


size_t
sc_next_power_of_two(size_t value)
{
    value -= 1;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return value + 1;
}

int
sc_is_power_of_two(size_t value)
{
    return (value & (value - 1)) == 0;
}
