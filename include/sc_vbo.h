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
#define SC_LIGHT_BUFFER_ID          3

/* flags that affect the vertex buffer object.  These are the effects:
    
   SC_VBO_LIGHT         a additional float is stored per vertex that
                        can hold light information.  The default value
                        is 1.0 which means fully lit.  An extra call
                        of sc_vbo_update_light_range can be used after
                        adding a triangle to update the value.
   SC_VBO_3D_TEXTURE    3D texture coordinates are used.  By default
                        the value for z will be 1.0 but it can bec
                        changed by assigning the texture coordinates
                        from an actual 3D texture with the help of
                        the sc_vbo_update_texcoords functions.
    SC_VBO_DYNAMIC      the VBO is flagged as dynamic on the graphics
                        device instead of the default static setting. */
#define SC_VBO_LIGHT                1
#define SC_VBO_3D_TEXTURE           2
#define SC_VBO_DYNAMIC              4

/* internally a vbo provides more information, but this is not part of the
   public interface and subject to change. */
struct _sc_vbo;
typedef struct _sc_vbo sc_vbo_t;

#define SC_VBO_HEADER \
    GLuint buffers[3]; \
    size_t vertices; \
    int flags

#ifndef _SC_DONT_DEFINE_VBO
struct _sc_vbo {
    SC_VBO_HEADER;
};
#endif

/* creates a new vbo waiting for triangles */
sc_vbo_t *sc_new_vbo(int flags);

/* frees the vbo.  This will remove the vertex buffer from the graphics
   device memory as well as main memory in case some data is still there */
void sc_free_vbo(sc_vbo_t *vbo);

/* finalizes the vbo and moves all the data to the graphics device */
void sc_vbo_finalize(sc_vbo_t *vbo);

/* reopens the vbo to add new data.  That way the vbo is not deleted
   from the device but replaced by new uploaded data.  vertices are
   set back to zero count, vbo has to refilled. */
void sc_vbo_reuse(sc_vbo_t *vbo, int flags);

/* adds a new triangle to the vbo */
void sc_vbo_add_triangle(sc_vbo_t *vbo, const sc_vec3_t *vertices,
                         const sc_vec3_t *normals,
                         const sc_vec2_t *tex_coords);

/* updates the texture coordiates from a texture.  This scales the x and
   y coordinates with the given texture offsets and sets the z index to
   the texture's index in case the dimension is 3 */
void sc_vbo_update_texcoords(sc_vbo_t *vbo, const sc_texture_t *tex);

/* like sc_vbo_update_texcoords but with additional range. */
void sc_vbo_update_texcoords_range(sc_vbo_t *vbo, int start, int end,
                                   const sc_texture_t *tex);

/* sets the light information for all vertices to a new value */
void sc_vbo_update_light(sc_vbo_t *vbo, float light);

/* light sc_vbo_update_light but with an additional range. */
void sc_vbo_update_light_range(sc_vbo_t *vbo, int start, int end, float light);

/* draws the complete vbo */
void sc_vbo_draw(const sc_vbo_t *vbo);

#endif
