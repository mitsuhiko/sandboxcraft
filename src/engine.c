#include "sc_engine.h"
#include "sc_error.h"

/* TODO: move to config */
#define WIDTH 800
#define HEIGHT 600
#define FULLSCREEN 0
#define FOV 55

static void
resize_viewport()
{
    Uint32 flags = SDL_OPENGL;
    if (FULLSCREEN)
        flags |= SDL_FULLSCREEN;
    if (!SDL_SetVideoMode(WIDTH, HEIGHT, 32, flags))
        sc_critical_error(SC_ESDL, SDL_GetError());

    /* activate opengl features */
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    /* setup projection */
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (GLfloat)WIDTH / HEIGHT, 1.0, 1000.0);

    /* setup modelview */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void
init_graphics()
{
    /* double buffering and MSAA */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    /* color channel sizes */
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    resize_viewport();
}

void
sc_engine_init(void)
{
    Uint32 sdl_flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
    if (SDL_Init(sdl_flags) < 0)
        sc_critical_error(SC_ESDL, SDL_GetError());

    init_graphics();
}

void
sc_engine_shutdown(void)
{
    SDL_Quit();
}
