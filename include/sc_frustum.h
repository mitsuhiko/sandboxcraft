/* helpers for the frustum calculations */
#ifndef _INC_SC_FRUSTUM_H_
#define _INC_SC_FRUSTUM_H_

#include "sc_boot.h"
#include "sc_vec4.h"
#include "sc_mat4.h"

#define SC_LEFT_FRUSTUM_PLANE       0
#define SC_RIGHT_FRUSTUM_PLANE      1
#define SC_BOTTOM_FRUSTUM_PLANE     2
#define SC_TOP_FRUSTUM_PLANE        3
#define SC_NEAR_FRUSTUM_PLANE       4
#define SC_FAR_FRUSTUM_PLANE        5

typedef struct {
    sc_vec4_t planes[6];
} sc_frustum_t;

/* sets the frustum to the current values from the engine */
sc_frustum_t *sc_get_current_frustum(sc_frustum_t *frustum_out);

/* tests if the given axis aligned bounding box is in the frustum.
   Return value:

     -1       entirely outside of frustum
     0        partially in the frustum
     1        completely inside the frustum */
int sc_frustum_test_aabb(const sc_frustum_t *frustum, const sc_vec3_t *vec1,
                         const sc_vec3_t *vec2);

#endif
