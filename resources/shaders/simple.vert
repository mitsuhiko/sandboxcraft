#include "common.shader"
varying vec3 N, L;

void
main(void)
{
    gl_Position = sc_mvp_matrix * vec4(sc_vertex, 1.0);
    N = sc_normal_matrix * sc_normal;
    L = vec3(0.3, 3.0, 10.0);
}
