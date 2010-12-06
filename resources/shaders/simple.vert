#include "common.shader"
varying vec3 N, L, P;

void
main(void)
{
    gl_Position = sc_mvp_matrix * gl_Vertex;
    P = gl_Position.xyz;
    N = sc_normal_matrix * gl_Normal;
    L = sc_normal_matrix * vec3(0.3, 0.8, -0.7);
}
