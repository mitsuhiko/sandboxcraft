#include "sc_vec2.h"
#include "sc_vec3.h"
#include "sc_shading.h"

#define INITIAL_SIZE 255

#define MODE_FRESH 0
#define MODE_FINALIZED 1
#define MODE_REUESD 2

#define _SC_DONT_DEFINE_VBO
#include "sc_vbo.h"

struct _sc_vbo {
    SC_VBO_HEADER;
    size_t _buffer_size;
    int _mode;
    float *_vertices;
    float *_normals;
    float *_tex_coords;
};

sc_vbo_t *
sc_new_vbo(int texcoord_dimension)
{
    sc_vbo_t *rv = sc_xalloc(sc_vbo_t);
    assert(texcoord_dimension >= 2 && texcoord_dimension <= 3);
    rv->texcoord_dimension = texcoord_dimension;
    rv->vertices = 0;
    rv->_mode = MODE_FRESH;
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
    if (vbo->_mode != MODE_FRESH)
        glDeleteBuffers(3, vbo->buffers);
    sc_free(vbo->_vertices);
    sc_free(vbo->_normals);
    sc_free(vbo->_tex_coords);
    sc_free(vbo);
}

void
sc_vbo_finalize(sc_vbo_t *vbo, int dynamic)
{
    GLenum mode = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
    assert(vbo->_mode != MODE_FINALIZED);

    if (vbo->_mode == MODE_FRESH)
        glGenBuffers(3, vbo->buffers);

    /* vertex data */
    glBindBuffer(GL_ARRAY_BUFFER, vbo->buffers[SC_VERTEX_BUFFER_ID]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vbo->vertices * 3,
                 vbo->_vertices, mode);

    /* normal data */
    glBindBuffer(GL_ARRAY_BUFFER, vbo->buffers[SC_NORMAL_BUFFER_ID]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vbo->vertices * 3,
                 vbo->_normals, mode);

    /* texture coordinates */
    glBindBuffer(GL_ARRAY_BUFFER, vbo->buffers[SC_TEXCOORD_BUFFER_ID]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * vbo->vertices * vbo->texcoord_dimension,
                 vbo->_tex_coords, mode);

    sc_free(vbo->_vertices);
    sc_free(vbo->_normals);
    sc_free(vbo->_tex_coords);

    vbo->_mode = MODE_FINALIZED;
    vbo->_vertices = NULL;
    vbo->_normals = NULL;
    vbo->_tex_coords = NULL;
}

void
sc_vbo_reuse(sc_vbo_t *vbo, int texcoord_dimension)
{
    assert(vbo->_mode == MODE_FINALIZED);
    assert(texcoord_dimension >= 2 && texcoord_dimension <= 3);
    vbo->_mode = MODE_REUESD;
    vbo->texcoord_dimension = texcoord_dimension;
    vbo->vertices = 0;
    vbo->_buffer_size = 0;
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

    vbo->_vertices = sc_xrealloc(vbo->_vertices, sizeof(float) * size * 3);
    vbo->_normals = sc_xrealloc(vbo->_normals, sizeof(float) * size * 3);
    vbo->_tex_coords = sc_xrealloc(vbo->_tex_coords, sizeof(float) * size *
                                   vbo->texcoord_dimension);
    vbo->_buffer_size = size;
}

void
sc_vbo_add_triangle(sc_vbo_t *vbo, const sc_vec3_t *vertices,
                    const sc_vec3_t *normals,
                    const sc_vec2_t *tex_coords)
{
    int i;
    void *texoff;
    sc_vec3_t normal;

    for (i = 0; i < 3; i++) {
        enlarge_if_necessary(vbo);
        texoff = vbo->_tex_coords + vbo->vertices * vbo->texcoord_dimension;
        sc_vec3_normalize(&normal, &normals[i]);
        *(sc_vec3_t *)(vbo->_vertices + vbo->vertices * 3) = vertices[i];
        *(sc_vec3_t *)(vbo->_normals + vbo->vertices * 3) = normal;
        *(sc_vec2_t *)(texoff) = tex_coords[i];
        if (vbo->texcoord_dimension == 3)
            *(float *)(texoff + sizeof(float) * 2) = 0.0f;
        vbo->vertices++;
    }
}

void
sc_vbo_update_texcoords_range(sc_vbo_t *vbo, int start, int end,
                              const sc_texture_t *tex)
{
    int i;
    float fac_x, fac_y, off_x, off_y;
    assert(vbo->_mode != MODE_FINALIZED);

    fac_x = (float)tex->width / tex->stored_width;
    fac_y = (float)tex->height / tex->stored_height;
    off_x = (float)tex->off_x / tex->stored_width;
    off_y = (float)tex->off_y / tex->stored_height;

    for (i = start; i < end; i++) {
        size_t off = vbo->texcoord_dimension * i;
        vbo->_tex_coords[off] = vbo->_tex_coords[off] * fac_x + off_x;
        vbo->_tex_coords[off + 1] = vbo->_tex_coords[off + 1] * fac_y + off_y;
        if (vbo->texcoord_dimension == 3)
            vbo->_tex_coords[off + 2] = (float)tex->index;
    }
}

void
sc_vbo_update_texcoords(sc_vbo_t *vbo, const sc_texture_t *tex)
{
    return sc_vbo_update_texcoords_range(vbo, 0, vbo->vertices, tex);
}

void
sc_vbo_draw(const sc_vbo_t *vbo)
{
    const char *texcoord_uniform = "sc_texcoord";
    assert(vbo->_mode == MODE_FINALIZED);
    if (!vbo->vertices)
        return;

    sc_floatb_attrib(NULL, "sc_vertex", 3, vbo->buffers[SC_VERTEX_BUFFER_ID]);
    sc_floatb_attrib(NULL, "sc_normal", 3, vbo->buffers[SC_NORMAL_BUFFER_ID]);
    if (vbo->texcoord_dimension == 3)
        texcoord_uniform = "sc_texcoord3";
    sc_floatb_attrib(NULL, texcoord_uniform, vbo->texcoord_dimension,
                     vbo->buffers[SC_TEXCOORD_BUFFER_ID]);

    glDrawArrays(GL_TRIANGLES, 0, vbo->vertices);

    sc_disable_buffer_attrib(NULL, texcoord_uniform);
    sc_disable_buffer_attrib(NULL, "sc_normal");
    sc_disable_buffer_attrib(NULL, "sc_vertex");
}
