varying vec3 N, L;
varying vec2 coord;
uniform sampler2D sc_texture;

#ifdef SC_VERTEX_SHADER
void
main(void)
{
    coord = sc_texcoord;
    gl_Position = sc_mvp_matrix * vec4(sc_vertex, 1.0);
    N = sc_normal_matrix * sc_normal;
    L = vec3(0.3, 3.0, 10.0);
}
#endif

#ifdef SC_FRAGMENT_SHADER
void
main(void)
{
    float intensity = max(0.0, dot(normalize(N), normalize(L)));
    gl_FragColor = texture2D(sc_texture, coord);
    gl_FragColor.rgb *= intensity;
}
#endif
