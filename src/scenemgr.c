#include "sc_scenemgr.h"
#include "sc_engine.h"

struct _sc_scenemgr {
    sc_list_t *lights;
    sc_list_t *cameras;
    sc_camera_t *active_cam;
    sc_world_t *world;
};

sc_scenemgr_t *
sc_new_scenemgr(void)
{
    sc_scenemgr_t *rv = sc_alloc(sc_scenemgr_t);
    rv->lights = sc_new_list();
    rv->cameras = sc_new_list();
    rv->active_cam = sc_scenemgr_create_camera(rv);
    rv->world = NULL;
    return rv;
}

void
sc_free_scenemgr(sc_scenemgr_t *mgr)
{
    size_t i;
    if (!mgr)
        return;

    for (i = 0; i < mgr->lights->size; i++)
        sc_free_light(mgr->lights->items[i]);
    sc_free_list(mgr->lights);

    for (i = 0; i < mgr->cameras->size; i++)
        sc_free_camera(mgr->cameras->items[i]);

    sc_free(mgr);
}

sc_light_t *
sc_scenemgr_create_light(sc_scenemgr_t *mgr)
{
    sc_light_t *light = sc_new_light();
    sc_list_append(mgr->lights, light);
    return light;
}

int
sc_scenemgr_remove_light(sc_scenemgr_t *mgr, sc_light_t *light)
{
    ssize_t idx = sc_list_find(mgr->lights, light);
    if (idx < 0)
        return 0;
    sc_free_light(mgr->lights->items[idx]);
    sc_list_delete(mgr->lights, idx);
    return 1;
}

const sc_list_t *
sc_scenemgr_get_lights(const sc_scenemgr_t *mgr)
{
    return mgr->lights;
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
    int i, light_num = 0;

    sc_engine_clear(sc_color(0x103B47ff));

    if (mgr->active_cam)
        sc_camera_apply(mgr->active_cam);

    for (i = 0; i < mgr->lights->size && light_num < 8; i++) {
        const sc_light_t *light = mgr->lights->items[i];
        if (light->enabled)
            sc_light_apply(light, light_num++);
    }
}

void
sc_scenemgr_end(const sc_scenemgr_t *mgr)
{
    sc_light_disable_all();
}

void
sc_scenemgr_draw(const sc_scenemgr_t *mgr)
{
    sc_scenemgr_begin(mgr);
    if (mgr->world)
        sc_world_draw(mgr->world, mgr->active_cam);
    sc_scenemgr_end(mgr);
}
