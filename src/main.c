#include "sc_boot.h"
#include "sc_game.h"
#include "sc_engine.h"
#include "sc_path.h"
#include "sc_mat4.h"

/* if we want to clean up, clean up on shutdown, otherwise do nothing */
#ifndef SC_DONT_FREE_ON_SHUTDOWN
#  define FREE_ON_SHUTDOWN(x) atexit(x)
#else
#  define FREE_ON_SHUTDOWN(x) (void)0
#endif

static void
write_config_if_changed(void)
{
    if (sc_config.dirty)
        sc_save_config();
}

static int
run_game_controlled(int argc, char **argv)
{
    sc_load_config();
    atexit(write_config_if_changed);
    sc_engine_init();
    FREE_ON_SHUTDOWN(sc_engine_shutdown);
    FREE_ON_SHUTDOWN(sc_game_shutdown);
    sc_game_mainloop();
    return 0;
}


#if SC_PLATFORM == SC_PLATFORM_WINDOWS
INT WINAPI
WinMain(HINSTANCE hinst, HINSTANCE previnst, LPSTR cmdline, INT cmdshow)
{
    return run_game_controlled(__argc, __argv);
}

/* On OS X this function is called client_main and the real main function
   is in the os x launcher which is a (not so simple) obj-c app */
#elif SC_PLATFORM == SC_PLATFORM_OSX
int
client_main(int argc, char **argv)
{
    return run_game_controlled(argc, argv);
}

/* on linux just call into run_game_controlled */
#elif SC_PLATFORM == SC_PLATFORM_LINUX
int
main(int argc, char **argv)
{
    return run_game_controlled(argc, argv);
}
#endif
