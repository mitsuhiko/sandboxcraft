/* this includes various lighting utilty functions */
#ifndef _SC_SHADER_LIGHTING_
#define _SC_SHADER_LIGHTING_

/* updates diffuse and specular as if they were hit with
   directional light of some specific values.  Directional light
   is light from a light source far away, similar to the sun */
void
directional_light(in vec3 normal,
                  in vec3 light_dir,
                  in vec3 half_vec,
                  in float shininess,
                  in vec4 light_diffuse,
                  in vec4 light_specular,
                  inout vec4 diffuse,
                  inout vec4 specular)
{
    float n_dot_vp;     /* normal . light dir */
    float n_dot_hv;     /* normal . half vec */
    float pf;           /* power factor */

    n_dot_vp = max(0.0, dot(normal, normalize(light_dir)));
    n_dot_hv = max(0.0, dot(normal, half_vec));

    pf = (n_dot_vp == 0.0) ? 0.0 : pow(n_dot_hv, shininess);

    diffuse += light_diffuse * n_dot_vp;
    specular += light_specular * pf;
}

#endif
