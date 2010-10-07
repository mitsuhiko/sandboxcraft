#include <assert.h>

#include "sc_engine.h"
#include "sc_error.h"

/* TODO: move to config */
#define WIDTH 800
#define HEIGHT 600
#define FULLSCREEN 0
#define FPS_LIMIT 60

sc_gametime_t sc_gametime;
static int mouse_grabbed;

static void
resize_viewport(void)
{
    Uint32 flags = SDL_OPENGL;
    if (FULLSCREEN)
        flags |= SDL_FULLSCREEN;
    if (!SDL_SetVideoMode(WIDTH, HEIGHT, 32, flags))
        sc_critical_error(SC_ESDL, __FILE__, __LINE__, "%s", SDL_GetError());

    SDL_WM_SetCaption("SandboxCraft", NULL);

    /* activate opengl features */
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    /* setup viewport */
    glViewport(0, 0, WIDTH, HEIGHT);
}

static void
init_graphics(void)
{
    /* double buffering and MSAA */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    /* seems to cause issues on my linux box. */
#if SC_PLATFORM != SC_PLATFORM_LINUX
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 2);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#endif

    /* color channel sizes */
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    resize_viewport();
}

static void
init_glut(void)
{
#if SC_PLATFORM != SC_PLATFORM_OSX
    int argc = 0;
    char **argv = NULL;
    glutInit(&argc, argv);
#endif
}

void
sc_engine_init(void)
{
    Uint32 sdl_flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
    if (SDL_Init(sdl_flags) < 0)
        sc_critical_error(SC_ESDL, __FILE__, __LINE__, "%s", SDL_GetError());

    /* init glut too for the time being because it's helpful during
       development. */
    init_glut();

    init_graphics();
}

void
sc_engine_shutdown(void)
{
    SDL_Quit();
}

void
sc_engine_get_dimensions(float *width, float *height)
{
    *width = WIDTH;
    *height = HEIGHT;
}

float
sc_engine_get_aspect(void)
{
    return (float)WIDTH / HEIGHT;
}

void
sc_engine_grab_mouse(int value)
{
    if (value) {
        SDL_WM_GrabInput(SDL_GRAB_ON);
        SDL_WarpMouse(0, 0);
        SDL_GetRelativeMouseState(NULL, NULL);
        SDL_ShowCursor(0);
        mouse_grabbed = 1;
    }
    else {
        SDL_WM_GrabInput(SDL_GRAB_OFF);
        SDL_ShowCursor(1);
        mouse_grabbed = 0;
    }
}

int
sc_engine_mouse_grabbed(void)
{
    return mouse_grabbed;
}

void
sc_engine_begin_frame(void)
{
    sc_gametime.start = sc_engine_get_ticks();
}

void
sc_engine_end_frame(void)
{
#ifdef FPS_LIMIT
    if (sc_gametime.delta < (1000.0f / FPS_LIMIT))
        sc_engine_delay((sc_ticks_t)((1000.0f / FPS_LIMIT) -
                                     sc_gametime.delta));
#endif

    sc_gametime.end = sc_engine_get_ticks();
    sc_gametime.delta = sc_gametime.end - sc_gametime.start;

    sc_engine_swap_buffers();
}
