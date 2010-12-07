varying vec3 N, L;

void
main(void)
{
    float intensity = max(0.0, dot(normalize(N), normalize(L)));
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    gl_FragColor.rgb *= intensity;
}
