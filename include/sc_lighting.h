/* general lighting system.  Currently uses the opengl stack for light
   handling but the idea is that at one point this could work with shaders */
#ifndef _INC_SC_LIGHTING_H_
#define _INC_SC_LIGHTING_H_

#include "sc_boot.h"
#include "sc_color.h"
#include "sc_vec3.h"
#include "sc_vec4.h"

typedef struct {
    sc_color_t ambient;
    sc_color_t diffuse;
    sc_color_t specular;
    sc_vec4_t position;
    sc_vec3_t spot_direction;
    float spot_exponent;
    float spot_cutoff;
    float constant_attenuation;
    float linear_attenuation;
    float quadratic_attenuation;
    int enabled;
} sc_light_t;

/* creates a new light */
sc_light_t *sc_new_light(void);

/* deletes a light again */
void sc_free_light(sc_light_t *light);

/* sends the light information to the graphics device.  Currently this sends
   it to the fixed function pipeline, but it could also send it to
   dedicated uniforms in case shaders are used.  In the hardware the light
   is represented with the integer light_num */
void sc_light_apply(const sc_light_t *light, int light_num);

/* disables all light channels again.  This disables all hardware light
   channels in the fixed function pipeline or later in the shader. */
void sc_light_disable_all(void);

#endif
