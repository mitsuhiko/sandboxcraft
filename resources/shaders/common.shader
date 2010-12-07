/* this include is added automatically to all vertex and fragment shaders.  You
   don't have to include it, but it also doesn't do any damage due to the
   include guard here. */
#ifndef _SC_SHADER_COMMON_
#define _SC_SHADER_COMMON_

/* common uniforms for the matrices.  These are always sent to the shader */
uniform mat4 sc_model_matrix;
uniform mat4 sc_view_matrix;
uniform mat4 sc_modelview_matrix;
uniform mat4 sc_projection_matrix;
uniform mat4 sc_mvp_matrix;
uniform mat3 sc_normal_matrix;
uniform sampler2D sc_texture;

/* same for attributes.  These are set when a vertex buffer object is drawn.
   We only add these if we are a vertex shader. */
#ifdef SC_VERTEX_SHADER
attribute vec3 sc_vertex;
attribute vec3 sc_normal;
attribute vec2 sc_texcoord;
#endif

#endif
