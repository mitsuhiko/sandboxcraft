/* various shader utilities */
#ifndef _SC_SHADER_UTILS_
#define _SC_SHADER_UTILS_

vec3
gamma(vec3 color)
{
    return pow(color, vec3(0.5));
}

vec3
fog(vec3 color, vec3 fcolor, float depth, float density)
{
    const float e = 2.718281828459045;
    return mix(fcolor, color, pow(e, -pow(depth * density, 2)));
}

#endif
