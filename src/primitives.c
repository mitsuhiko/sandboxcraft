#include "sc_primitives.h"


sc_vbo_t *
sc_new_cube(float size)
{
    sc_vbo_t *rv = sc_new_vbo();

    sc_cube_add_front_plane(rv, size, 0.0f, 0.0f, 0.0f);
    sc_cube_add_back_plane(rv, size, 0.0f, 0.0f, 0.0f);
    sc_cube_add_top_plane(rv, size, 0.0f, 0.0f, 0.0f);
    sc_cube_add_bottom_plane(rv, size, 0.0f, 0.0f, 0.0f);
    sc_cube_add_left_plane(rv, size, 0.0f, 0.0f, 0.0f);
    sc_cube_add_right_plane(rv, size, 0.0f, 0.0f, 0.0f);

    return rv;
}

void
sc_cube_add_front_plane(sc_vbo_t *vbo, float size, float off_x,
                        float off_y, float off_z)
{
    sc_vec3_t vertices[3];
    sc_vec3_t normals[3];
    sc_vec2_t tex_coords[3];
    float x = size / 2;

    sc_vec3_set(&vertices[0], off_x + x, off_y + x, off_z + x);
    sc_vec3_set(&vertices[1], off_x - x, off_y + x, off_z + x);
    sc_vec3_set(&vertices[2], off_x - x, off_y - x, off_z + x);
    sc_vec3_set(&normals[0], 0.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[1], 0.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[2], 0.0f, 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[1], 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 0.0f);
    sc_vbo_add_triangle(vbo, vertices, normals, tex_coords);

    sc_vec3_set(&vertices[0], off_x + x, off_y - x, off_z + x);
    sc_vec3_set(&vertices[1], off_x + x, off_y + x, off_z + x);
    sc_vec3_set(&vertices[2], off_x - x, off_y - x, off_z + x);
    sc_vec3_set(&normals[0], 0.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[1], 0.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[2], 0.0f, 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 0.0f);
    sc_vbo_add_triangle(vbo, vertices, normals, tex_coords);
}

void
sc_cube_add_back_plane(sc_vbo_t *vbo, float size, float off_x,
                       float off_y, float off_z)
{
    sc_vec3_t vertices[3];
    sc_vec3_t normals[3];
    sc_vec2_t tex_coords[3];
    float x = size / 2;

    sc_vec3_set(&vertices[0], off_x - x, off_y - x, off_z - x);
    sc_vec3_set(&vertices[1], off_x - x, off_y + x, off_z - x);
    sc_vec3_set(&vertices[2], off_x + x, off_y + x, off_z - x);
    sc_vec3_set(&normals[0], 0.0f, 0.0f, -1.0f);
    sc_vec3_set(&normals[1], 0.0f, 0.0f, -1.0f);
    sc_vec3_set(&normals[2], 0.0f, 0.0f, -1.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 1.0f);
    sc_vbo_add_triangle(vbo, vertices, normals, tex_coords);

    sc_vec3_set(&vertices[0], off_x - x, off_y - x, off_z - x);
    sc_vec3_set(&vertices[1], off_x + x, off_y + x, off_z - x);
    sc_vec3_set(&vertices[2], off_x + x, off_y - x, off_z - x);
    sc_vec3_set(&normals[0], 0.0f, 0.0f, -1.0f);
    sc_vec3_set(&normals[1], 0.0f, 0.0f, -1.0f);
    sc_vec3_set(&normals[2], 0.0f, 0.0f, -1.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 0.0f);
    sc_vbo_add_triangle(vbo, vertices, normals, tex_coords);
}

void
sc_cube_add_top_plane(sc_vbo_t *vbo, float size, float off_x,
                      float off_y, float off_z)
{
    sc_vec3_t vertices[3];
    sc_vec3_t normals[3];
    sc_vec2_t tex_coords[3];
    float x = size / 2;

    sc_vec3_set(&vertices[0], off_x - x, off_y + x, off_z - x);
    sc_vec3_set(&vertices[1], off_x - x, off_y + x, off_z + x);
    sc_vec3_set(&vertices[2], off_x + x, off_y + x, off_z + x);
    sc_vec3_set(&normals[0], 1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[1], 1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[2], 1.0f, 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 1.0f);
    sc_vbo_add_triangle(vbo, vertices, normals, tex_coords);

    sc_vec3_set(&vertices[0], off_x - x, off_y + x, off_z - x);
    sc_vec3_set(&vertices[1], off_x + x, off_y + x, off_z + x);
    sc_vec3_set(&vertices[2], off_x + x, off_y + x, off_z - x);
    sc_vec3_set(&normals[0], 1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[1], 1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[2], 1.0f, 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 0.0f);
    sc_vbo_add_triangle(vbo, vertices, normals, tex_coords);
}

void
sc_cube_add_bottom_plane(sc_vbo_t *vbo, float size, float off_x,
                         float off_y, float off_z)
{
    sc_vec3_t vertices[3];
    sc_vec3_t normals[3];
    sc_vec2_t tex_coords[3];
    float x = size / 2;

    sc_vec3_set(&vertices[0], off_x + x, off_y - x, off_y + x);
    sc_vec3_set(&vertices[1], off_x - x, off_y - x, off_y + x);
    sc_vec3_set(&vertices[2], off_x - x, off_y - x, off_y - x);
    sc_vec3_set(&normals[0], -1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[1], -1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[2], -1.0f, 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[1], 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 0.0f);
    sc_vbo_add_triangle(vbo, vertices, normals, tex_coords);

    sc_vec3_set(&vertices[0], off_x + x, off_y - x, off_y - x);
    sc_vec3_set(&vertices[1], off_x + x, off_y - x, off_y + x);
    sc_vec3_set(&vertices[2], off_x - x, off_y - x, off_y - x);
    sc_vec3_set(&normals[0], -1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[1], -1.0f, 0.0f, 0.0f);
    sc_vec3_set(&normals[2], -1.0f, 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 0.0f);
    sc_vbo_add_triangle(vbo, vertices, normals, tex_coords);
}

void
sc_cube_add_left_plane(sc_vbo_t *vbo, float size, float off_x,
                       float off_y, float off_z)
{
    sc_vec3_t vertices[3];
    sc_vec3_t normals[3];
    sc_vec2_t tex_coords[3];
    float x = size / 2;

    sc_vec3_set(&vertices[0], off_x - x, off_y - x, off_z - x);
    sc_vec3_set(&vertices[1], off_x - x, off_y - x, off_z + x);
    sc_vec3_set(&vertices[2], off_x - x, off_y + x, off_z + x);
    sc_vec3_set(&normals[0], -1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[1], -1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[2], -1.0f, 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[0], 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 1.0f, 0.0f);
    sc_vec2_set(&tex_coords[2], 1.0f, 1.0f);
    sc_vbo_add_triangle(vbo, vertices, normals, tex_coords);

    sc_vec3_set(&vertices[0], off_x - x, off_y - x, off_z - x);
    sc_vec3_set(&vertices[1], off_x - x, off_y + x, off_z + x);
    sc_vec3_set(&vertices[2], off_x - x, off_y + x, off_z - x);
    sc_vec3_set(&normals[0], -1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[1], -1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[2], -1.0f, 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[0], 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[1], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 0.0f, 1.0f);
    sc_vbo_add_triangle(vbo, vertices, normals, tex_coords);
}

void
sc_cube_add_right_plane(sc_vbo_t *vbo, float size, float off_x,
                        float off_y, float off_z)
{
    sc_vec3_t vertices[3];
    sc_vec3_t normals[3];
    sc_vec2_t tex_coords[3];
    float x = size / 2;

    sc_vec3_set(&vertices[0], off_x + x, off_y + x, off_z + x);
    sc_vec3_set(&vertices[1], off_x + x, off_y - x, off_z + x);
    sc_vec3_set(&vertices[2], off_x + x, off_y - x, off_z - x);
    sc_vec3_set(&normals[0], 1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[1], 1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[2], 1.0f, 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[0], 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[1], 0.0f, 0.0f);
    sc_vec2_set(&tex_coords[2], 1.0f, 0.0f);
    sc_vbo_add_triangle(vbo, vertices, normals, tex_coords);

    sc_vec3_set(&vertices[0], off_x + x, off_y + x, off_z - x);
    sc_vec3_set(&vertices[1], off_x + x, off_y + x, off_z + x);
    sc_vec3_set(&vertices[2], off_x + x, off_y - x, off_z - x);
    sc_vec3_set(&normals[0], 1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[1], 1.0f, 0.0f, 1.0f);
    sc_vec3_set(&normals[2], 1.0f, 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[0], 1.0f, 1.0f);
    sc_vec2_set(&tex_coords[1], 0.0f, 1.0f);
    sc_vec2_set(&tex_coords[2], 1.0f, 0.0f);
    sc_vbo_add_triangle(vbo, vertices, normals, tex_coords);
}
