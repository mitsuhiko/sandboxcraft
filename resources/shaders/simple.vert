/* simple shader for the world during development */
#include "lighting.shader"

varying vec3 normal, half_vec;
varying vec3 coord;

void
main(void)
{
    coord = sc_texcoord3;
    gl_Position = sc_mvp_matrix * vec4(sc_vertex, 1.0);
    normal = sc_normal_matrix * sc_normal;
    half_vec = normalize(sc_vertex + sc_sun_direction) / 2.0;
}
