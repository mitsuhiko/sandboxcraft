#include "sc_scenemgr.h"
#include "sc_engine.h"
#include "sc_shading.h"

struct _sc_scenemgr {
    sc_list_t *cameras;
    sc_camera_t *active_cam;
    sc_world_t *world;
    /* global lighting */
    sc_color_t ambient_color;
    sc_color_t sun_color;
    sc_vec3_t sun_direction;
};

sc_scenemgr_t *
sc_new_scenemgr(void)
{
    sc_scenemgr_t *rv = sc_alloc(sc_scenemgr_t);
    rv->cameras = sc_new_list();
    rv->active_cam = sc_scenemgr_create_camera(rv);
    rv->world = NULL;

    rv->ambient_color = sc_color(0x888888ff);
    rv->sun_color = sc_color(0xdbf3f1ff);
    sc_vec3_set(&rv->sun_direction, 0.6f, 0.4f, 1.0f);
    sc_vec3_normalize(&rv->sun_direction, &rv->sun_direction);

    return rv;
}

void
sc_free_scenemgr(sc_scenemgr_t *mgr)
{
    size_t i;
    if (!mgr)
        return;

    for (i = 0; i < mgr->cameras->size; i++)
        sc_free_camera(mgr->cameras->items[i]);

    sc_free(mgr);
}

sc_camera_t *
sc_scenemgr_create_camera(sc_scenemgr_t *mgr)
{
    sc_camera_t *camera = sc_new_camera();
    sc_list_append(mgr->cameras, camera);
    return camera;
}

int
sc_scenemgr_remove_camera(sc_scenemgr_t *mgr, sc_camera_t *camera)
{
    ssize_t idx = sc_list_find(mgr->cameras, camera);
    if (idx < 0)
        return 0;
    sc_free_camera(mgr->cameras->items[idx]);
    sc_list_delete(mgr->cameras, idx);
    return 1;
}

const sc_list_t *
sc_scenemgr_get_cameras(const sc_scenemgr_t *mgr)
{
    return mgr->cameras;
}

void
sc_scenemgr_set_active_camera(sc_scenemgr_t *mgr, sc_camera_t *cam)
{
    mgr->active_cam = cam;
}

sc_camera_t *
sc_scenemgr_get_active_camera(sc_scenemgr_t *mgr)
{
    return mgr->active_cam;
}

void
sc_scenemgr_set_world(sc_scenemgr_t *mgr, sc_world_t *world)
{
    mgr->world = world;
}

sc_world_t *
sc_scenemgr_get_world(sc_scenemgr_t *mgr)
{
    return mgr->world;
}

void
sc_scenemgr_begin(const sc_scenemgr_t *mgr)
{
    sc_engine_clear(sc_color(0x111111ff));

    if (mgr->active_cam)
        sc_camera_apply(mgr->active_cam);

    sc_color_uniform(NULL, "sc_ambient_color", mgr->ambient_color);
    sc_color_uniform(NULL, "sc_sun_color", mgr->sun_color);
    sc_vec3_uniform(NULL, "sc_sun_direction", &mgr->sun_direction);
}

void
sc_scenemgr_end(const sc_scenemgr_t *mgr)
{
}

void
sc_scenemgr_draw(const sc_scenemgr_t *mgr)
{
    sc_scenemgr_begin(mgr);
    if (mgr->world)
        sc_world_draw(mgr->world, mgr->active_cam);
    sc_scenemgr_end(mgr);
}
