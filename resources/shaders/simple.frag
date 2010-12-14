#include "lighting.shader"
#extension GL_EXT_texture_array : enable

varying vec3 normal, half_vec;
varying vec3 coord;
varying float vertex_light;
uniform sampler2DArray sc_texture;

void
main(void)
{
    vec4 darkness = vec4(0.1, 0.1, 0.1, 1.0);
    vec4 ambient = sc_ambient_color;
    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);
    vec4 color = texture2DArray(sc_texture, coord);

    directional_light(normal, sc_sun_direction, half_vec,
                      30.0, sc_sun_color, vec4(0.0),
                      diffuse, specular);

    color = color * clamp(darkness + (ambient + diffuse) * vertex_light, 0.0, 1.0);
    gl_FragColor = clamp(color, 0.0, 1.0);
}
