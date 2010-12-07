/* the sandboxcraft engine */
#ifndef _INC_SC_ENGINE_H_
#define _INC_SC_ENGINE_H_

#include "sc_boot.h"
#include "sc_color.h"
#include "sc_vec3.h"
#include "sc_mat3.h"
#include "sc_mat4.h"
#include "sc_ray.h"

/* abstract the sdl tick interface */
#define sc_engine_get_ticks SDL_GetTicks
#define sc_engine_delay SDL_Delay
typedef Uint32 sc_ticks_t;

typedef struct {
    sc_ticks_t delta;           /* time between last frames */
    sc_ticks_t start;           /* starttime of this frame */
    sc_ticks_t end;             /* endtime of this frame */
} sc_gametime_t;

extern sc_gametime_t sc_gametime;

/* same for the buffer swapping */
#define sc_engine_swap_buffers SDL_GL_SwapBuffers

/* initializes the engine.  If that does not work, a critical error is set
   and the application aborts. */
void sc_engine_init(void);
void sc_engine_shutdown(void);

/* writes the detected engine features to stderr */
void sc_engine_dump_info(void);

/* returns the version of OpenGL as string for debugging purposes.  This
   should not be used for feature checks in the code however.  Use the
   GLee support macros instead */
const char *sc_engine_get_opengl_version(void);

/* returns the current dimensions of the viewport */
void sc_engine_get_dimensions(int *width, int *height);

/* returns the current view aspect */
float sc_engine_get_aspect(void);

/* enables/disables mouse grabbing */
void sc_engine_grab_mouse(int value);

/* is the mouse grabbed? */
int sc_engine_mouse_grabbed(void);

/* clears the screen.  This clears depth buffer and color. */
void sc_engine_clear(sc_color_t color);

/* returns the current model matrix */
sc_mat4_t *sc_engine_get_model_matrix(sc_mat4_t *mat_out);

/* sets a new model matrix */
void sc_engine_set_model_matrix(const sc_mat4_t *mat);

/* returns the current view matrix */
sc_mat4_t *sc_engine_get_view_matrix(sc_mat4_t *mat_out);

/* sets a new view matrix */
void sc_engine_set_view_matrix(const sc_mat4_t *mat);

/* returns the current projection matrix */
sc_mat4_t *sc_engine_get_projection_matrix(sc_mat4_t *mat_out);

/* sets a new projection matrix */
void sc_engine_set_projection_matrix(const sc_mat4_t *mat);

/* returns the current model-view matrix */
sc_mat4_t *sc_engine_get_modelview_matrix(sc_mat4_t *mat_out);

/* returns the current model-view-projection matrix */
sc_mat4_t *sc_engine_get_mvp_matrix(sc_mat4_t *mat_out);

/* returns the normal matrix */
sc_mat3_t *sc_engine_get_normal_matrix(sc_mat3_t *mat_out);

/* unprojects screen coordinates into 3d space.  It will use the z buffer
   information to reference the forcemost vertex. */
sc_vec3_t *sc_engine_unproject(sc_vec3_t *vec_out, int x, int y);

/* casts an actual ray into the screen */
sc_ray_t *sc_engine_raycast(sc_ray_t *ray_out, int x, int y);

/* callback for begin/end of frame handling */
void sc_engine_begin_frame(void);
void sc_engine_end_frame(void);

#endif
