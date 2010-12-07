#include "lighting.shader"

varying vec3 normal, half_vec;
varying vec2 coord;

void
main(void)
{
    vec4 ambient = sc_ambient_color;
    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);
    vec4 color = texture2D(sc_texture, coord);

    directional_light(normal, sc_sun_direction, half_vec,
                      30.0, sc_sun_color, vec4(0.0),
                      diffuse, specular);

    color = color * clamp(ambient + diffuse, 0.0, 1.0);
    gl_FragColor = clamp(color, 0.0, 1.0);
}
