#ifndef _INC_SC_CONFIG_H_
#define _INC_SC_CONFIG_H_

/* if this is defined, the game will not free up memory on shutdown that
   is freed only for the purpose of calling into free().  This makes for
   much faster shutdowns but makes it harder to debug using valgrind */
#define SC_DONT_FREE_ON_SHUTDOWN

#endif
