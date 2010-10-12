#include "sc_frustum.h"
#include "sc_engine.h"


sc_frustum_t *
sc_get_current_frustum(sc_frustum_t *frustum_out)
{
    sc_mat4_t mvp;
    sc_vec4_t *vec;

    sc_engine_get_mvp_matrix(&mvp);

    /* left plane */
    vec = &frustum_out->planes[SC_LEFT_FRUSTUM_PLANE];
    vec->x = mvp.elms[3] + mvp.elms[0];
    vec->y = mvp.elms[7] + mvp.elms[4];
    vec->z = mvp.elms[11] + mvp.elms[8];
    vec->w = mvp.elms[15] + mvp.elms[12];
    sc_vec4_normalize(vec);

    /* right plane */
    vec = &frustum_out->planes[SC_RIGHT_FRUSTUM_PLANE];
    vec->x = mvp.elms[3] - mvp.elms[0];
    vec->y = mvp.elms[7] - mvp.elms[4];
    vec->z = mvp.elms[11] - mvp.elms[8];
    vec->w = mvp.elms[15] - mvp.elms[12];
    sc_vec4_normalize(vec);

    /* bottom plane */
    vec = &frustum_out->planes[SC_BOTTOM_FRUSTUM_PLANE];
    vec->x = mvp.elms[3] + mvp.elms[1];
    vec->y = mvp.elms[7] + mvp.elms[5];
    vec->z = mvp.elms[11] + mvp.elms[9];
    vec->w = mvp.elms[15] + mvp.elms[13];
    sc_vec4_normalize(vec);

    /* top plane */
    vec = &frustum_out->planes[SC_TOP_FRUSTUM_PLANE];
    vec->x = mvp.elms[3] - mvp.elms[1];
    vec->y = mvp.elms[7] - mvp.elms[5];
    vec->z = mvp.elms[11] - mvp.elms[9];
    vec->w = mvp.elms[15] - mvp.elms[13];
    sc_vec4_normalize(vec);

    /* near plane */
    vec = &frustum_out->planes[SC_NEAR_FRUSTUM_PLANE];
    vec->x = mvp.elms[3] + mvp.elms[2];
    vec->y = mvp.elms[7] + mvp.elms[6];
    vec->z = mvp.elms[11] + mvp.elms[10];
    vec->w = mvp.elms[15] + mvp.elms[14];
    sc_vec4_normalize(vec);

    /* far plane */
    vec = &frustum_out->planes[SC_FAR_FRUSTUM_PLANE];
    vec->x = mvp.elms[3] - mvp.elms[2];
    vec->y = mvp.elms[7] - mvp.elms[6];
    vec->z = mvp.elms[11] - mvp.elms[10];
    vec->w = mvp.elms[15] - mvp.elms[14];
    sc_vec4_normalize(vec);

    return frustum_out;
}

static int
plane_test(const sc_vec4_t *plane, const sc_vec3_t *vec1,
           const sc_vec3_t *vec2)
{
    float p1 = vec1->x * plane->x;
    float p2 = vec1->y * plane->y;
    float p3 = vec1->z * plane->z;
    float d1 = vec2->x * plane->x;
    float d2 = vec2->y * plane->y;
    float d3 = vec2->z * plane->z;
    int points = 0;

    if (p1 + p2 + p3 + plane->w > 0) points++;
    if (p1 + p2 + d3 + plane->w > 0) points++;
    if (p1 + d2 + p3 + plane->w > 0) points++;
    if (p1 + d2 + d3 + plane->w > 0) points++;
    if (d1 + p2 + p3 + plane->w > 0) points++;
    if (d1 + p2 + d3 + plane->w > 0) points++;
    if (d1 + d2 + p3 + plane->w > 0) points++;
    if (d1 + d2 + d3 + plane->w > 0) points++;

    return points;
}

int
sc_frustum_test_aabb(const sc_frustum_t *frustum, const sc_vec3_t *vec1,
                     const sc_vec3_t *vec2)
{
    int i, rv, points_visible = 0;

    for (i = 0; i < 6; i++) {
        if ((rv = plane_test(&frustum->planes[i], vec1, vec2)) == 0)
            return -1;
        points_visible += rv;
    }

    return points_visible == 48;
}
