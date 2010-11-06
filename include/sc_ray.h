/* represents a ray */
#ifndef _INC_SC_RAY_H_
#define _INC_SC_RAY_H_

#include "sc_boot.h"
#include "sc_vec3.h"

typedef struct {
    sc_vec3_t pos;
    sc_vec3_t dir;
} sc_ray_t;

/* does this ray intersect a given AABB? */
int sc_ray_intersects_aabb(const sc_ray_t *ray, const sc_vec3_t *vec1,
                           const sc_vec3_t *vec2, float *distance_out);

#endif
