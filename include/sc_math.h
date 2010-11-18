/* math helpers */
#ifndef _INC_SC_MATH_H_
#define _INC_SC_MATH_H_

#include <math.h>

#include "sc_boot.h"

#define SC_PI 3.141592653589794f

static sc_inline float
sc_deg2rad(float deg)
{
    return ((deg) * SC_PI / 180.0f);
}

static sc_inline float
sc_rad2deg(float rad)
{
    return ((rad) * 180.0f / SC_PI);
}

size_t sc_next_power_of_two(size_t value);

static sc_inline int
sc_is_power_of_two(size_t value)
{
    return (value & (value - 1)) == 0;
}

#endif
