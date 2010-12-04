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
    sc_vec4_t half_vector;
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

#endif
