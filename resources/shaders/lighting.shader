/* this includes various lighting utilty functions */
#ifndef _SC_SHADER_LIGHTING_
#define _SC_SHADER_LIGHTING_

/* the struct for spherical harmonics coefficients */
struct shc_t {
    vec3 l00, l1m1, l10, l11, l2m2, l2m1, l20, l21, l22;
};

shc_t sh_groove = shc_t(
    vec3( 0.3783264,  0.4260425,  0.4504587),
    vec3( 0.2887813,  0.3586803,  0.4147053),
    vec3( 0.0379030,  0.0295216,  0.0098567),
    vec3(-0.1033028, -0.1031690, -0.0884924),
    vec3(-0.0621750, -0.0554432, -0.0396779),
    vec3( 0.0077820, -0.0148312, -0.0471301),
    vec3(-0.0935561, -0.1254260, -0.1525629),
    vec3(-0.0572703, -0.0502192, -0.0363410),
    vec3( 0.0203348, -0.0044201, -0.0452180)
);

shc_t sh_beach = shc_t(
    vec3( 0.6841148,  0.6929004,  0.7069543),
    vec3( 0.3173355,  0.3694407,  0.4406839),
    vec3(-0.1747193, -0.1737154, -0.1657420),
    vec3(-0.4496467, -0.4155184, -0.3416573),
    vec3(-0.1690202, -0.1703022, -0.1525870),
    vec3(-0.0837808, -0.0940454, -0.1027518),
    vec3(-0.0319670, -0.0214051, -0.0147691),
    vec3( 0.1641816,  0.1377558,  0.1010403),
    vec3( 0.3697189,  0.3097930,  0.2029923)
);

shc_t sh_tomb = shc_t(
    vec3( 1.0351604,  0.7603549,  0.7074635),
    vec3( 0.4442150,  0.3430402,  0.3403777),
    vec3(-0.2247797, -0.1828517, -0.1705181),
    vec3( 0.7110400,  0.5423169,  0.5587956),
    vec3( 0.6430452,  0.4971454,  0.5156357),
    vec3(-0.1150112, -0.0936603, -0.0839287),
    vec3(-0.3742487, -0.2755962, -0.2875017),
    vec3(-0.1694954, -0.1343096, -0.1335315),
    vec3( 0.5515260,  0.4222179,  0.4162488)
);

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

/* spherical harmonics lighting */
vec3
spherical_harmonics_light(vec3 normal, shc_t l)
{
    const float c1 = 0.429043;
    const float c2 = 0.511664;
    const float c3 = 0.743125;
    const float c4 = 0.886227;
    const float c5 = 0.247708;

    return (
        c1 * l.l22 * (normal.x * normal.x - normal.y * normal.y) +
        c3 * l.l20 * normal.z * normal.z +
        c4 * l.l00 -
        c5 * l.l20 +
        2.0 * c1 * l.l2m2 * normal.x * normal.y +
        2.0 * c1 * l.l21  * normal.x * normal.z +
        2.0 * c1 * l.l2m1 * normal.y * normal.z +
        2.0 * c2 * l.l11  * normal.x +
        2.0 * c2 * l.l1m1 * normal.y +
        2.0 * c2 * l.l10  * normal.z
    );
}

#endif
