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
    assert(light_num < 8);

#define SEND_FLOAT(Type, Ptr) glLightf(GL_LIGHT0 + light_num, Type, Ptr)
#define SEND_FLOATV(Type, Ptr) glLightfv(GL_LIGHT0 + light_num, Type, Ptr)
#define SEND_COLOR(Type, Color) do { \
    float color[4]; \
    sc_color_to_floatv(Color, color); \
    SEND_FLOATV(Type, color); \
} while (0)

    glEnable(GL_LIGHT0 + light_num);

    SEND_COLOR(GL_AMBIENT, light->ambient);
    SEND_COLOR(GL_DIFFUSE, light->diffuse);
    SEND_COLOR(GL_SPECULAR, light->specular);

    SEND_FLOATV(GL_POSITION, sc_vec4_ptr(&light->position));
    SEND_FLOATV(GL_SPOT_DIRECTION, sc_vec3_ptr(&light->spot_direction));
    SEND_FLOAT(GL_CONSTANT_ATTENUATION, light->constant_attenuation);
    SEND_FLOAT(GL_LINEAR_ATTENUATION, light->linear_attenuation);
    SEND_FLOAT(GL_QUADRATIC_ATTENUATION, light->quadratic_attenuation);
}

void
sc_light_disable_all(void)
{
    int i;
    for (i = 0; i < 8; i++)
        glDisable(GL_LIGHT0 + i);
}
