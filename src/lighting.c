#include "sc_lighting.h"

sc_light_t *
sc_new_light(void)
{
    sc_light_t *rv = sc_alloc(sc_light_t);
    memset(rv, 0, sizeof(sc_light_t));
    rv->enabled = 1;
    rv->constant_attenuation = 1.0f;
    return rv;
}

void
sc_free_light(sc_light_t *light)
{
    if (!light)
        return;
    sc_free(light);
}

void
sc_light_apply(const sc_light_t *light, int light_num)
{
}

void
sc_light_disable_all(void)
{
}
