/* math helpers */
#ifndef _INC_SC_MATH_H_
#define _INC_SC_MATH_H_

#include <math.h>

#include "sc_boot.h"

#define sc_deg2rad(deg) ((deg) * M_PI / 180.0f)
#define sc_rad2deg(rad) ((rad) * 180.0f / M_PI)

size_t sc_next_power_of_two(size_t value);
int sc_is_power_of_two(size_t value);

#endif
