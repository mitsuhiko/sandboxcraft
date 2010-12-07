/* low-level shading subsystem powered by GLSL.
 
   Example usage:
   
     sc_shader_t *sh = sc_new_shader();
     if (!sc_shader_attach_from_file(sh, "simple.vert", SC_VERTEX_SHADER) ||
         !sc_shader_attach_from_file(sh, "simple.frag", SC_FRAGMENT_SHADER) ||
         !sc_shader_finalize(sh))
         sc_error_make_critical();

   The above code can also be written in abbreviated form with the
   help of a builtin factory function which will automatically load
   *.vert files as vertex shaders and *.frag as fragment shaders.

     sc_shader_t *sh = sc_shader_from_file("simple");

   Binding of shaders:

     sc_shader_bind(shader);

   Unbinding:

     sc_shader_unbind();

   Deleting shaders:

     sc_free_shader(shader);

   */
#ifndef _INC_SC_SHADING_H_
#define _INC_SC_SHADING_H_

#include "sc_boot.h"
#include "sc_color.h"

struct _sc_vec2;
struct _sc_vec3;
struct _sc_vec4;
struct _sc_mat3;
struct _sc_mat4;
struct _sc_shader;
typedef struct _sc_shader sc_shader_t;

#define SC_VERTEX_SHADER        1
#define SC_FRAGMENT_SHADER      2

/* creates a new shader */
sc_shader_t *sc_new_shader(void);

/* simplified way to load shaders from files */
sc_shader_t *sc_shader_from_file(const char *basename);

/* frees a shader */
void sc_free_shader(sc_shader_t *shader);

/* attaches a new shader module */
int sc_shader_attach_from_file(sc_shader_t *shader, const char *filename,
                               int type);

/* links all modules in the shader */
int sc_shader_finalize(sc_shader_t *shader);

/* binds a shader */
void sc_shader_bind(const sc_shader_t *shader);

/* unbinds currently bound shader */
void sc_shader_unbind(void);

/* returns the currently bound shader.  All uniform and attribute functions
   will automatically operate on the active shader if NULL is provided. */
const sc_shader_t *sc_get_current_shader(void);

/* sends all kinds of uniforms */
void sc_int_uniform(const sc_shader_t *shader, const char *name, int val);
void sc_float_uniform(const sc_shader_t *shader, const char *name, float val);
void sc_color_uniform(const sc_shader_t *shader, const char *name, sc_color_t val);
void sc_vec2_uniform(const sc_shader_t *shader, const char *name, const struct _sc_vec2 *vec);
void sc_vec3_uniform(const sc_shader_t *shader, const char *name, const struct _sc_vec3 *vec);
void sc_vec4_uniform(const sc_shader_t *shader, const char *name, const struct _sc_vec4 *vec);
void sc_mat3_uniform(const sc_shader_t *shader, const char *name, const struct _sc_mat3 *mat);
void sc_mat4_uniform(const sc_shader_t *shader, const char *name, const struct _sc_mat4 *mat);

/* queries all kinds of uniforms */
int sc_get_int_uniform(const sc_shader_t *shader, const char *name);
float sc_get_float_uniform(const sc_shader_t *shader, const char *name);
sc_color_t sc_get_color_uniform(const sc_shader_t *shader, const char *name);
struct _sc_vec2 *sc_get_vec2_uniform(const sc_shader_t *shader, const char *name, struct _sc_vec2 *vec_out);
struct _sc_vec3 *sc_get_vec3_uniform(const sc_shader_t *shader, const char *name, struct _sc_vec3 *vec_out);
struct _sc_vec4 *sc_get_vec4_uniform(const sc_shader_t *shader, const char *name, struct _sc_vec4 *vec_out);
struct _sc_mat3 *sc_get_mat3_uniform(const sc_shader_t *shader, const char *name, struct _sc_mat3 *mat_out);
struct _sc_mat4 *sc_get_mat4_uniform(const sc_shader_t *shader, const char *name, struct _sc_mat4 *mat_out);

/* sends all kinds of vertex attributes */
void sc_float_attrib(const sc_shader_t *shader, const char *name, float val);
void sc_color_attrib(const sc_shader_t *shader, const char *name, sc_color_t val);
void sc_vec2_attrib(const sc_shader_t *shader, const char *name, const struct _sc_vec2 *vec);
void sc_vec3_attrib(const sc_shader_t *shader, const char *name, const struct _sc_vec3 *vec);
void sc_vec4_attrib(const sc_shader_t *shader, const char *name, const struct _sc_vec4 *vec);
void sc_floatb_attrib(const sc_shader_t *shader, const char *name, int size, GLuint buffer);
void sc_disable_buffer_attrib(const sc_shader_t *shader, const char *name);

/* queries all kinds of attributes */
float sc_get_float_attrib(const sc_shader_t *shader, const char *name);
sc_color_t sc_get_color_attrib(const sc_shader_t *shader, const char *name);
struct _sc_vec2 *sc_get_vec2_attrib(const sc_shader_t *shader, const char *name, struct _sc_vec2 *vec_out);
struct _sc_vec3 *sc_get_vec3_attrib(const sc_shader_t *shader, const char *name, struct _sc_vec3 *vec_out);
struct _sc_vec4 *sc_get_vec4_attrib(const sc_shader_t *shader, const char *name, struct _sc_vec4 *vec_out);

#endif
