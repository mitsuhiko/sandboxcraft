#include "common.shader"
varying vec3 N, L, P;

void
main(void)
{
    gl_Position = sc_mvp_matrix * vec4(sc_vertex, 1.0);
    P = gl_Position.xyz;
    N = sc_normal_matrix * sc_normal;
    L = sc_normal_matrix * vec3(0.3, 0.8, -0.7);
}
