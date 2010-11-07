/* debug support for the engine */
#ifndef _INC_SC_DEBUG_H_
#define _INC_SC_DEBUG_H_

#include "sc_boot.h"
#include "sc_vec3.h"

/* executes a wrapped expression only if in debug mode */
#ifdef NDEBUG
#  define SC_IF_DEBUG(E) (void)0
#else
#  define SC_IF_DEBUG(E) E
#endif

/* draws a line on the screen from start to end */
void _sc_debug_add_line(const sc_vec3_t *start, const sc_vec3_t *end);
#define sc_debug_add_line(S, E) SC_IF_DEBUG(_sc_debug_add_line(S, E))
#define sc_debug_add_line2(SX, SY, SZ, EX, EY, EZ) SC_IF_DEBUG(do { \
    sc_vec3_t start, end; \
    sc_vec3_set(&start, SX, SY, SZ); \
    sc_vec3_set(&end, EX, EY, EZ); \
    _sc_debug_add_line(&start, &end); \
} while(0))

/* draws a line on the screen from start with a given direction and length */
void _sc_debug_add_line_with_direction(const sc_vec3_t *start,
                                       const sc_vec3_t *direction,
                                       float length);
#define sc_debug_add_line_with_direction(S, D, L) \
    SC_IF_DEBUG(_sc_debug_add_line_with_direction(S, D, L))
#define sc_debug_add_line_with_direction2(SX, SY, SZ, DX, DY, DZ, L) \
    SC_IF_DEBUG(do { \
    sc_vec3_t start, direction; \
    sc_vec3_set(&start, SX, SY, SZ); \
    sc_vec3_set(&direction, DX, DY, DZ); \
    _sc_debug_add_line_with_direction(&start, &end, L); \
} while(0))

/* draws a ray to the screen */
#define sc_debug_add_ray(R, L) \
    sc_debug_add_line_with_direction(&(R)->pos, &(R)->dir, L)

/* flushes the debug draws to screen.  In debug mode this is automatically
   called right before the engine ends the frame rendering but it could be
   useful to flush earlier.  Note that in non debug builds the debug
   add functions are noops however calling into the ones with underscore
   prefixes will still fill the debug buffer. */
void sc_debug_flush(void);

/* removes all debug infos again */
void sc_debug_clear(void);

#endif
