#include "sc_lighting.h"

sc_light_t *
sc_new_light(void)
{
    sc_light_t *rv = sc_alloc(sc_light_t);
    memset(rv, 0, sizeof(sc_light_t));
    rv->enabled = 1;
    rv->position.z = 1.0f;
    rv->ambient = sc_color(0x222222ff);
    return rv;
}

void
sc_free_light(sc_light_t *light)
{
    if (!light)
        return;
    sc_free(light);
}
