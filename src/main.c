#include <stdlib.h>

#include "sc_boot.h"
#include "sc_game.h"
#include "sc_engine.h"
#include "sc_path.h"
#include "sc_mat4.h"

static void
test_matrix(void)
{
    sc_mat4_t mat;
    sc_mat4_set(&mat,
        2, 1, 4, 3,
        1, 1, 0, 0,
        1, 1, 1, 0,
        1, 1, 1, 1
    );
    sc_mat4_inverse(&mat, &mat);
    sc_mat4_debug(&mat);
}

static int
run_game_controlled(int argc, char **argv)
{
#if 0
    test_matrix();
    return 0;
#endif
    sc_engine_init();
    atexit(sc_engine_shutdown);
    sc_game_early_init();
    atexit(sc_game_shutdown);
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
