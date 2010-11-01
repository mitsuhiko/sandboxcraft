#include <assert.h>
#include "sc_vec2.h"
#include "sc_vec3.h"

#define INITIAL_SIZE 255
#define ASSERT_FINALIZED() assert(vbo->_finalized)
#define ASSERT_NOT_FINALIZED() assert(!vbo->_finalized)

typedef struct {
    GLuint buffers[4];
    size_t vertices;
    size_t _buffer_size;
    int _finalized;
    /* this is where the vertices are stored in main memory until they
       are uploaded to the graphics device.  The first three items above
       are the only ones exposed in the header, everything down here is
       specific to the implementation. */
    sc_vec3_t *_vertices;
    sc_vec3_t *_normals;
    sc_vec2_t *_tex_coords;
} sc_vbo_t;

/* internally a vbo is more than what we expose */
#define _SC_DONT_DEFINE_VBO
#include "sc_vbo.h"

sc_vbo_t *
sc_new_vbo(void)
{
    sc_vbo_t *rv = sc_xalloc(sc_vbo_t);
    rv->vertices = 0;
    rv->_finalized = 0;
    rv->_buffer_size = 0;
    rv->_vertices = NULL;
    rv->_normals = NULL;
    rv->_tex_coords = NULL;
    return rv;
}

void
sc_free_vbo(sc_vbo_t *vbo)
{
    if (!vbo)
        return;
    sc_free(vbo->_vertices);
    sc_free(vbo->_normals);
    sc_free(vbo->_tex_coords);
    sc_free(vbo);
}

void
sc_finalize_vbo(sc_vbo_t *vbo)
{
    ASSERT_NOT_FINALIZED();
    if (!vbo->vertices)
        goto finalize;

    glGenBuffers(4, vbo->buffers);

    /* vertex data */
    glBindBuffer(GL_ARRAY_BUFFER, vbo->buffers[SC_VERTEX_BUFFER_ID]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vbo->vertices * 3,
                 vbo->_vertices, GL_STATIC_DRAW);

    /* normal data */
    glBindBuffer(GL_ARRAY_BUFFER, vbo->buffers[SC_NORMAL_BUFFER_ID]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vbo->vertices * 3,
                 vbo->_normals, GL_STATIC_DRAW);

    /* texture coordinates */
    glBindBuffer(GL_ARRAY_BUFFER, vbo->buffers[SC_TEXCOORD_BUFFER_ID]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vbo->vertices * 2,
                 vbo->_tex_coords, GL_STATIC_DRAW);

finalize:
    sc_free(vbo->_vertices);
    sc_free(vbo->_normals);
    sc_free(vbo->_tex_coords);

    vbo->_finalized = 1;
    vbo->_vertices = NULL;
    vbo->_normals = NULL;
    vbo->_tex_coords = NULL;
}

static void
enlarge_if_necessary(sc_vbo_t *vbo)
{
    size_t size;

    if (vbo->vertices < vbo->_buffer_size)
        return;

    size = !vbo->_buffer_size ? INITIAL_SIZE : (size_t)(vbo->_buffer_size * 1.3);

    vbo->_vertices = sc_xrealloc(vbo->_vertices, sizeof(sc_vec3_t) * size * 3);
    vbo->_normals = sc_xrealloc(vbo->_normals, sizeof(sc_vec3_t) * size * 3);
    vbo->_tex_coords = sc_xrealloc(vbo->_tex_coords, sizeof(sc_vec3_t) * size * 2);
    vbo->_buffer_size = size;
}

void
sc_vbo_add_triangle(sc_vbo_t *vbo, const sc_vec3_t *vertices,
                    const sc_vec3_t *normals,
                    const sc_vec2_t *tex_coords)
{
    int i;
    sc_vec3_t normal;

    for (i = 0; i < 3; i++) {
        normal = normals[i];
        sc_vec3_normalize(&normal);
        enlarge_if_necessary(vbo);
        memcpy(vbo->_vertices + vbo->vertices, &vertices[i], sizeof(sc_vec3_t));
        memcpy(vbo->_normals + vbo->vertices, &normal, sizeof(sc_vec3_t));
        memcpy(vbo->_tex_coords + vbo->vertices, &tex_coords[i], sizeof(sc_vec2_t));
        vbo->vertices++;
    }
}

void
sc_vbo_update_texcoords_range(sc_vbo_t *vbo, int start, int end,
                              float offset_x, float offset_y,
                              float factor_x, float factor_y)
{
    int i;
    ASSERT_NOT_FINALIZED();

    for (i = start; i < end; i++) {
        vbo->_tex_coords[i].x = vbo->_tex_coords[i].x * factor_x + offset_x;
        vbo->_tex_coords[i].y = vbo->_tex_coords[i].y * factor_y + offset_y;
    }
}

void
sc_vbo_update_texcoords(sc_vbo_t *vbo, float offset_x, float offset_y,
                        float factor_x, float factor_y)
{
    return sc_vbo_update_texcoords_range(vbo, 0, vbo->vertices,
                                         offset_x, offset_y,
                                         factor_x, factor_y);
}

void
sc_vbo_update_texcoords_from_texture_range(sc_vbo_t *vbo, int start, int end,
                                           const sc_texture_t *texture)
{
    sc_vbo_update_texcoords_range(vbo, start, end,
                                  texture->coords[0], texture->coords[1],
                                  texture->coords[2] - texture->coords[0],
                                  texture->coords[5] - texture->coords[1]);
}

void
sc_vbo_update_texcoords_from_texture(sc_vbo_t *vbo, const sc_texture_t *texture)
{
    sc_vbo_update_texcoords_from_texture_range(vbo, 0, vbo->vertices, texture);
}

void
sc_vbo_draw(const sc_vbo_t *vbo)
{
    ASSERT_FINALIZED();
    if (!vbo->vertices)
        return;

    /* vertex data */
    glBindBuffer(GL_ARRAY_BUFFER, vbo->buffers[SC_VERTEX_BUFFER_ID]);
    glVertexPointer(3, GL_FLOAT,0,  0);
    /* normal data */
    glBindBuffer(GL_ARRAY_BUFFER, vbo->buffers[SC_NORMAL_BUFFER_ID]);
    glNormalPointer(GL_FLOAT, 0, 0);
    /* texture coordinates */
    glBindBuffer(GL_ARRAY_BUFFER, vbo->buffers[SC_TEXCOORD_BUFFER_ID]);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, vbo->vertices);
}
