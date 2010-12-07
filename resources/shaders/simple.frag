varying vec3 N, L;
varying vec2 coord;

void
main(void)
{
    float intensity = max(0.0, dot(normalize(N), normalize(L)));
    gl_FragColor = texture2D(sc_texture, coord);
    gl_FragColor.rgb *= intensity;
}
