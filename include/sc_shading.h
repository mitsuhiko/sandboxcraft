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

struct _sc_shader;
typedef struct _sc_shader sc_shader_t;

typedef enum {
    SC_VERTEX_SHADER,
    SC_FRAGMENT_SHADER
} sc_shadertype_t;

/* creates a new shader */
sc_shader_t *sc_new_shader(void);

/* simplified way to load shaders from files */
sc_shader_t *sc_shader_from_file(const char *basename);

/* frees a shader */
void sc_free_shader(sc_shader_t *shader);

/* attaches a new shader module */
int sc_shader_attach_from_file(sc_shader_t *shader, const char *filename,
                               sc_shadertype_t type);

/* links all modules in the shader */
int sc_shader_finalize(sc_shader_t *shader);

/* binds a shader */
void sc_shader_bind(const sc_shader_t *shader);

/* unbinds currently bound shader */
void sc_shader_unbind(void);

#endif
