/* scene manager that simplifies memory management keeping track of objects
   added to a scene. */
#ifndef _INC_SC_SCENEMGR_H_
#define _INC_SC_SCENEMGR_H_

#include "sc_boot.h"
#include "sc_color.h"
#include "sc_camera.h"
#include "sc_world.h"
#include "sc_list.h"

struct _sc_scenemgr;
typedef struct _sc_scenemgr sc_scenemgr_t;

/* creates a new scene manager */
sc_scenemgr_t *sc_new_scenemgr(void);

/* frees the scene manager */
void sc_free_scenemgr(sc_scenemgr_t *mgr);

/* creates a new camera.  A default camera is created automatically. */
sc_camera_t *sc_scenemgr_create_camera(sc_scenemgr_t *mgr);

/* removes a camera from the scene manager again */
int sc_scenemgr_remove_camera(sc_scenemgr_t *mgr, sc_camera_t *camera);

/* returns a constant list of all cameras */
const sc_list_t *sc_scenemgr_get_cameras(const sc_scenemgr_t *mgr);

/* sets the new active camera */
void sc_scenemgr_set_active_camera(sc_scenemgr_t *mgr, sc_camera_t *cam);

/* returns the currently active camera */
sc_camera_t *sc_scenemgr_get_active_camera(sc_scenemgr_t *mgr);

/* sets the active world for the scene manager.  The memory belongs to
   someone else, it is not freed on the scene manager's free */
void sc_scenemgr_set_world(sc_scenemgr_t *mgr, sc_world_t *world);

/* returns the currently referenced world if there is one */
sc_world_t *sc_scenemgr_get_world(sc_scenemgr_t *mgr);

/* begins the scene rendering.  This sends the active camera coordinates to
   the engine and other things */
void sc_scenemgr_begin(const sc_scenemgr_t *mgr);

/* reverts some things of begin. */
void sc_scenemgr_end(const sc_scenemgr_t *mgr);

/* draws the complete scene.  Assumes the world is attached */
void sc_scenemgr_draw(const sc_scenemgr_t *mgr);

#endif
