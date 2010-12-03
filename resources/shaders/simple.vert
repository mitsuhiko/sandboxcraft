varying vec3 N, L, P;

void
main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    P = gl_Position.xyz;
    N = gl_NormalMatrix * gl_Normal;
    L = gl_NormalMatrix * vec3(0.3, 0.8, -0.7);
}
