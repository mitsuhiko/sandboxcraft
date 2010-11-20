/* represents a ray */
#ifndef _INC_SC_RAY_H_
#define _INC_SC_RAY_H_

#include "sc_boot.h"
#include "sc_vec3.h"

typedef struct {
    sc_vec3_t pos;
    sc_vec3_t dir;
} sc_ray_t;

#define SC_LEFT_AABB_SIDE       0
#define SC_RIGHT_AABB_SIDE      1
#define SC_BOTTOM_AABB_SIDE     2
#define SC_TOP_AABB_SIDE        3
#define SC_NEAR_AABB_SIDE       4
#define SC_FAR_AABB_SIDE        5
#define SC_INSIDE_AABB          6

/* does this ray intersect a given AABB?  side_out is set to one of the
   SC_*_AABB_SIDE constants or SC_INSIDE_AABB if the ray is already
   inside the AABB.  Both distance_out and side_out can be set to NULL
   in which case no information is passed out. */
int sc_ray_intersects_aabb(const sc_ray_t *ray, const sc_vec3_t *vec1,
                           const sc_vec3_t *vec2, float *distance_out,
                           int *side_out);

#endif
