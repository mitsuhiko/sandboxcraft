/* implements basic support for vertex buffer objects.

   The vbo interface that is implemented here always operates on triangles
   and will internally optimize for best memory usage by comparing with
   already registered vertices.  This however also means that the add
   method has a complexity of O(n) which slows down the vertex buffer
   creation.
   
   It might be interesting to check if the check in there is really
   necessary. */
#ifndef _INC_SC_VBO_H_
#define _INC_SC_VBO_H_

#include "sc_boot.h"
#include "sc_vec2.h"
#include "sc_vec3.h"
#include "sc_texture.h"

/* refers to a specific buffer in the buffers array */
#define SC_VERTEX_BUFFER_ID         0
#define SC_NORMAL_BUFFER_ID         1
#define SC_TEXCOORD_BUFFER_ID       2

/* internally a vbo provides more information, but this is not part of the
   public interface and subject to change.  Due to how the implementation
   looks like we have to not define this type if we are asked for it. */
#ifndef _SC_DONT_DEFINE_VBO
typedef struct {
    GLuint buffers[3];
    size_t vertices;
} sc_vbo_t;
#endif

/* creates a new vbo waiting for triangles */
sc_vbo_t *sc_new_vbo(void);

/* frees the vbo.  This will remove the vertex buffer from the graphics
   device memory as well as main memory in case some data is still there */
void sc_free_vbo(sc_vbo_t *vbo);

/* finalizes the vbo and moves all the data to the graphics device */
void sc_finalize_vbo(sc_vbo_t *vbo);

/* reopens the vbo to add new data.  That way the vbo is not deleted
   from the device but replaced by new uploaded data.  vertices are
   set back to zero count, vbo has to refilled. */
void sc_reuse_vbo(sc_vbo_t *vbo);

/* adds a new triangle to the vbo */
void sc_vbo_add_triangle(sc_vbo_t *vbo, const sc_vec3_t *vertices,
                         const sc_vec3_t *normals,
                         const sc_vec2_t *tex_coords);

/* updates the texture coordiates.  This is intended for use with texture
   atlasses and probably only useful for quad-ish textures. */
void sc_vbo_update_texcoords(sc_vbo_t *vbo, float offset_x,
                             float offset_y, float factor_x, float factor_y);

/* like sc_vbo_update_texcoords but with additional range */
void sc_vbo_update_texcoords_range(sc_vbo_t *vbo, int start, int end,
                                   float offset_x, float offset_y,
                                   float factor_x, float factor_y);

/* shortcut for sc_vbo_update_texcoords when texture coordinates should be
   updated from the texture coordinates of a texture (for use with atlasses) */
void sc_vbo_update_texcoords_from_texture(sc_vbo_t *vbo, const sc_texture_t *texture);

/* like sc_vbo_update_texcoords_from_texture but with a range */
void sc_vbo_update_texcoords_from_texture_range(sc_vbo_t *vbo, int start, int end,
                                                const sc_texture_t *texture);

/* draws the complete vbo */
void sc_vbo_draw(const sc_vbo_t *vbo);

#endif
