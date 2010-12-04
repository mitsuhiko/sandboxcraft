/* scene manager that simplifies memory management keeping track of objects
   added to a scene. */
#ifndef _INC_SC_SCENEMGR_H_
#define _INC_SC_SCENEMGR_H_

#include "sc_boot.h"
#include "sc_lighting.h"

struct _sc_scenemgr;
typedef struct _sc_scenemgr sc_scenemgr_t;

/* creates a new scene manager */
sc_scenemgr_t *sc_new_scenemgr(void);

/* frees the scene manager */
void sc_free_scenemgr(sc_scenemgr_t *mgr);

/* creates a new light in the scene that is then managed in the manager */
sc_light_t *sc_scenemgr_create_light(sc_scenemgr_t *mgr);

/* removes a light from the scene manager again.  Returns 0 if that light
   is not managed by the scene manager. */
int sc_scenemgr_remove_light(sc_scenemgr_t *mgr, sc_light_t *light);

#endif
