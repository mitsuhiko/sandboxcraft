#include "sc_primitives.h"


sc_vbo_t *
sc_new_cube(float size)
{
    sc_vec3_t vertices[3];
    sc_vec3_t normals[3];
    sc_vec2_t tex_coords[3];
    float x = size / 2;
    sc_vbo_t *rv = sc_new_vbo();

    /* front plane */
    sc_vec3_set(&vertices[0], x, x, x);
    sc_vec3_set(&vertices[1], -x, x, x);
    sc_vec3_set(&vertices[2], -x, -x, x);
    sc_vec3_set(&normals[0], 0.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[1], 0.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[2], 0.0f, 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[1], 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 0.0f);
    sc_vbo_add_triangle(rv, vertices, normals, tex_coords);

    sc_vec3_set(&vertices[0], x, -x, x);
    sc_vec3_set(&vertices[1], x, x, x);
    sc_vec3_set(&vertices[2], -x, -x, x);
    sc_vec3_set(&normals[0], 0.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[1], 0.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[2], 0.0f, 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 0.0f);
    sc_vbo_add_triangle(rv, vertices, normals, tex_coords);

    /* back plane */
    sc_vec3_set(&vertices[0], -x, -x, -x);
    sc_vec3_set(&vertices[1], -x, x, -x);
    sc_vec3_set(&vertices[2], x, x, -x);
    sc_vec3_set(&normals[0], 0.0f, 0.0f, -1.0f);
    sc_vec3_set(&normals[1], 0.0f, 0.0f, -1.0f);
    sc_vec3_set(&normals[2], 0.0f, 0.0f, -1.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 1.0f);
    sc_vbo_add_triangle(rv, vertices, normals, tex_coords);

    sc_vec3_set(&vertices[0], -x, -x, -x);
    sc_vec3_set(&vertices[1], x, x, -x);
    sc_vec3_set(&vertices[2], x, -x, -x);
    sc_vec3_set(&normals[0], 0.0f, 0.0f, -1.0f);
    sc_vec3_set(&normals[1], 0.0f, 0.0f, -1.0f);
    sc_vec3_set(&normals[2], 0.0f, 0.0f, -1.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 0.0f);
    sc_vbo_add_triangle(rv, vertices, normals, tex_coords);

    /* bottom plane */
    sc_vec3_set(&vertices[0], x, -x, x);
    sc_vec3_set(&vertices[1], -x, -x, x);
    sc_vec3_set(&vertices[2], -x, -x, -x);
    sc_vec3_set(&normals[0], -1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[1], -1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[2], -1.0f, 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[1], 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 0.0f);
    sc_vbo_add_triangle(rv, vertices, normals, tex_coords);

    sc_vec3_set(&vertices[0], x, -x, -x);
    sc_vec3_set(&vertices[1], x, -x, x);
    sc_vec3_set(&vertices[2], -x, -x, -x);
    sc_vec3_set(&normals[0], -1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[1], -1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[2], -1.0f, 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 0.0f);
    sc_vbo_add_triangle(rv, vertices, normals, tex_coords);

    /* top plane */
    sc_vec3_set(&vertices[0], -x, x, -x);
    sc_vec3_set(&vertices[1], -x, x, x);
    sc_vec3_set(&vertices[2], x, x, x);
    sc_vec3_set(&normals[0], 1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[1], 1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[2], 1.0f, 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 1.0f);
    sc_vbo_add_triangle(rv, vertices, normals, tex_coords);

    sc_vec3_set(&vertices[0], -x, x, -x);
    sc_vec3_set(&vertices[1], x, x, x);
    sc_vec3_set(&vertices[2], x, x, -x);
    sc_vec3_set(&normals[0], 1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[1], 1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[2], 1.0f, 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 0.0f);
    sc_vbo_add_triangle(rv, vertices, normals, tex_coords);

    /* left plane */
    sc_vec3_set(&vertices[0], -x, -x, -x);
    sc_vec3_set(&vertices[1], -x, -x, x);
    sc_vec3_set(&vertices[2], -x, x, x);
    sc_vec3_set(&normals[0], -1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[1], -1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[2], -1.0f, 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[0], 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[2], 1.0f, 1.0f);
    sc_vbo_add_triangle(rv, vertices, normals, tex_coords);

    sc_vec3_set(&vertices[0], -x, -x, -x);
    sc_vec3_set(&vertices[1], -x, x, x);
    sc_vec3_set(&vertices[2], -x, x, -x);
    sc_vec3_set(&normals[0], -1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[1], -1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[2], -1.0f, 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[0], 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 1.0f);
    sc_vbo_add_triangle(rv, vertices, normals, tex_coords);

    /* right plane */
    sc_vec3_set(&vertices[0], x, x, x);
    sc_vec3_set(&vertices[1], x, -x, x);
    sc_vec3_set(&vertices[2], x, -x, -x);
    sc_vec3_set(&normals[0], 1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[1], 1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[2], 1.0f, 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[0], 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[1], 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[2], 1.0f, 0.0f);
    sc_vbo_add_triangle(rv, vertices, normals, tex_coords);

    sc_vec3_set(&vertices[0], x, x, -x);
    sc_vec3_set(&vertices[1], x, x, x);
    sc_vec3_set(&vertices[2], x, -x, -x);
    sc_vec3_set(&normals[0], 1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[1], 1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[2], 1.0f, 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[1], 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 1.0f, 0.0f);
    sc_vbo_add_triangle(rv, vertices, normals, tex_coords);

    sc_finalize_vbo(rv);
    return rv;
}
