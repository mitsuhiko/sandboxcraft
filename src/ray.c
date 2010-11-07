#include "sc_ray.h"


int
sc_ray_intersects_aabb(const sc_ray_t *ray, const sc_vec3_t *vec1,
                       const sc_vec3_t *vec2, float *distance_out)
{
    float t, lowt = 0.0f;
    int did_hit = 0;
    sc_vec3_t hit;

    /* simple case: origin is inside the AABB */
    if (ray->pos.x > vec1->x && ray->pos.y > vec1->y && ray->pos.z > vec1->z &&
        ray->pos.x < vec2->x && ray->pos.y < vec2->y && ray->pos.z < vec2->z)
        goto hit;

#define HIT_CHECK(Vec, S, Max, SA, SB) do { \
    if (Max \
        ? ray->pos.S >= Vec->S && ray->dir.S < 0 \
        : ray->pos.S <= Vec->S && ray->dir.S > 0) { \
        t = (vec1->S - ray->pos.S) / ray->dir.S; \
        if (t >= 0) { \
            sc_vec3_add(&hit, &ray->pos, &ray->dir); \
            sc_vec3_mul(&hit, &hit, t); \
            if ((!did_hit || t < lowt) && \
                hit.SA >= vec1->SA && hit.SA <= vec2->SA && \
                hit.SB >= vec1->SB && hit.SB <= vec2->SB) { \
                did_hit = 1; \
                lowt = t; \
            } \
        } \
    } \
} while (0)

    HIT_CHECK(vec1, x, 0, y, z);    /* min x */
    HIT_CHECK(vec2, x, 1, y, z);    /* max x */
    HIT_CHECK(vec1, y, 0, x, z);    /* min y */
    HIT_CHECK(vec2, y, 1, x, z);    /* max y */
    HIT_CHECK(vec1, z, 0, x, y);    /* min z */
    HIT_CHECK(vec2, z, 1, x, y);    /* max z */

    if (!did_hit)
        return 0;

hit:
    if (distance_out)
        *distance_out = lowt;
    return 1;
}
