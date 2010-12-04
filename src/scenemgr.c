#include "sc_scenemgr.h"
#include "sc_list.h"

struct _sc_scenemgr {
    sc_list_t *lights;
};

sc_scenemgr_t *
sc_new_scenemgr(void)
{
    sc_scenemgr_t *rv = sc_alloc(sc_scenemgr_t);
    rv->lights = sc_new_list();
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
