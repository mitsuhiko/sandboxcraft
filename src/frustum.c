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
plane_test(const sc_vec4_t *plane, const sc_bounding_box_t *box)
{
    float p1 = box->position.x * plane->x;
    float p2 = box->position.y * plane->y;
    float p3 = box->position.z * plane->z;
    float d1 = box->dimensions.x * plane->x;
    float d2 = box->dimensions.y * plane->y;
    float d3 = box->dimensions.z * plane->z;
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
sc_frustum_test(const sc_frustum_t *frustum, const sc_bounding_box_t *box)
{
    int i, rv, points_visible = 0;

    for (i = 0; i < 6; i++) {
        if ((rv = plane_test(&frustum->planes[i], box)) == 0)
            return -1;
        points_visible += rv;
    }

    return points_visible == 48;
}
