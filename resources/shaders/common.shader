#ifndef _SC_SHADER_COMMON_
#define _SC_SHADER_COMMON_

/* common uniforms for the matrices.  These are always sent to the shader */
uniform mat4 sc_model_matrix;
uniform mat4 sc_view_matrix;
uniform mat4 sc_modelview_matrix;
uniform mat4 sc_projection_matrix;
uniform mat4 sc_mvp_matrix;
uniform mat3 sc_normal_matrix;

/* same for attributes.  These are set when a vertex buffer object is drawn */
attribute vec3 sc_vertex;
attribute vec3 sc_normal;
attribute vec2 sc_texcoord;

#endif
