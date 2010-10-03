/* the sandboxcraft engine */
#ifndef _INC_SC_ENGINE_H_
#define _INC_SC_ENGINE_H_

#include "sc_boot.h"

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

/* engine management */
void sc_engine_init(void);
void sc_engine_shutdown(void);

/* callback for begin/end of frame handling */
void sc_engine_begin_frame(void);
void sc_engine_end_frame(void);

#endif
