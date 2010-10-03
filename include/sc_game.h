/* the sandboxcraft engine */
#ifndef _INC_SC_GAME_H_
#define _INC_SC_GAME_H_

#include "sc_boot.h"

void sc_game_init(void);
void sc_game_shutdown(void);
void sc_game_mainloop(void);
void sc_game_stop(void);
void sc_game_handle_event(SDL_Event *evt);
void sc_game_render(void);

#endif
