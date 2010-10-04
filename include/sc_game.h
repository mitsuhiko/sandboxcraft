/* the sandboxcraft engine */
#ifndef _INC_SC_GAME_H_
#define _INC_SC_GAME_H_

#include "sc_boot.h"
#include "sc_engine.h"

/* init that happens right after graphics system is loaded */
void sc_game_early_init(void);

/* init that happens in a loading screen */
void sc_game_late_init(void);

/* shutdown's early and late init, but late must not have happened yet */
void sc_game_shutdown(void);

void sc_game_mainloop();
void sc_game_stop(void);

void sc_game_handle_events(void);
void sc_game_handle_event(SDL_Event *evt);
void sc_game_update(void);
void sc_game_render(void);

#endif
