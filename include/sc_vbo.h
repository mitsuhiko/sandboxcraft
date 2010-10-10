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

#define SC_VERTEX_BUFFER_ID         0
#define SC_NORMAL_BUFFER_ID         1
#define SC_TEXCOORD_BUFFER_ID       2
#define SC_INDEX_BUFFER_ID          3

/* internally a vbo provides more information, but this is not part of the
   public interface and subject to change.  Due to how the implementation
   looks like we have to not define this type if we are asked for it. */
#ifndef _SC_DONT_DEFINE_VBO
typedef struct {
    GLuint buffers[4];
    size_t indices;
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

/* adds a new triangle to the vbo */
void sc_vbo_add_triangle(sc_vbo_t *vbo, const sc_vec3_t *vertices[3],
                         const sc_vec3_t *normals[3],
                         const sc_vec2_t *tex_coords[3]);

/* draws the complete vbo */
void sc_vbo_draw(const sc_vbo_t *vbo);

#endif
