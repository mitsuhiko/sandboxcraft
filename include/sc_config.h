/* provides access to compile time config as well as the actual game and
   engine configuration */
#ifndef _INC_SC_CONFIG_H_
#define _INC_SC_CONFIG_H_

/* if this is defined, the game will not free up memory on shutdown that
   is freed only for the purpose of calling into free().  This makes for
   much faster shutdowns but makes it harder to debug using valgrind */
#define SC_DONT_FREE_ON_SHUTDOWN

struct _sc_config {
    /* [video] */
    int screen_width;
    int screen_height;
    int fullscreen;
    int fov;
    int ansitropic_filtering;

    int dirty;                  /* was the config changed? */
};

extern struct _sc_config sc_config;

/* loads the config from the filesystem. */
int sc_load_config(void);

/* writes the config back to the filesystem */
int sc_save_config(void);

#endif
