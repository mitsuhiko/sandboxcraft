/* the sandboxcraft engine */
#ifndef _INC_SC_ENGINE_H_
#define _INC_SC_ENGINE_H_

#include "sc_boot.h"
#include "sc_color.h"

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

/* returns the current dimensions of the viewport */
void sc_engine_get_dimensions(float *width, float *height);

/* returns the current view aspect */
float sc_engine_get_aspect(void);

/* enables/disables mouse grabbing */
void sc_engine_grab_mouse(int value);

/* is the mouse grabbed? */
int sc_engine_mouse_grabbed(void);

/* clears the screen.  This clears depth buffer and color. */
void sc_engine_clear(sc_color_t color);

/* callback for begin/end of frame handling */
void sc_engine_begin_frame(void);
void sc_engine_end_frame(void);

#endif
