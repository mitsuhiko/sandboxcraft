/* this include is added automatically to all vertex and fragment shaders.  You
   don't have to include it, but it also doesn't do any damage due to the
   include guard here. */
#ifndef _SC_SHADER_COMMON_
#define _SC_SHADER_COMMON_

/* common uniforms for the matrices.  These are always sent to the shader by
   various parts in the pipeline.  The matrices are set when the shader is
   bound, the sc_texture is set when a texture is bound.  Due to the nature
   of how this works, the texture must be bound after the shader was bound. */
uniform mat4 sc_model_matrix;
uniform mat4 sc_view_matrix;
uniform mat4 sc_modelview_matrix;
uniform mat4 sc_projection_matrix;
uniform mat4 sc_mvp_matrix;
uniform mat3 sc_normal_matrix;

/* same for attributes.  These are set when a vertex buffer object is drawn.
   We only add these if we are a vertex shader. */
#ifdef SC_VERTEX_SHADER
attribute vec3 sc_vertex;
attribute vec3 sc_normal;
attribute vec2 sc_texcoord;
attribute vec3 sc_texcoord3;
#endif

/* note on sc_texture: because the samplers are of different kind and not known
   at compile time in a generic fashing the fragment shader has to set it itself
   accordingly (for instance uniform sampler2D sc_texture); */

/* the sunlight and global light information from the scene manager.  These
   are available when the rendering was triggered from the scene manager's
   draw function */
uniform vec4 sc_ambient_color;
uniform vec4 sc_sun_color;
uniform vec3 sc_sun_direction;

#endif
