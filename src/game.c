#include "sc_game.h"

static int running;


static void
iterate_mainloop(void)
{
    /* handle events */
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
        if (evt.type == SDL_QUIT)
            sc_game_stop();
        else
            sc_game_handle_event(&evt);
    }
    sc_game_render();
}

void
sc_game_handle_event(SDL_Event *evt)
{
    if (evt->type == SDL_KEYDOWN &&
        evt->key.keysym.sym == SDLK_ESCAPE)
        sc_game_stop();
}

void
sc_game_render(void)
{
    /* clear display first and load identity matrix into model view */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* draw something nice */
    glTranslatef(0.0f, 0.0f, -300.0f);
    glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCube(50.0);

    /* swap buffers */
    SDL_GL_SwapBuffers();
}

void
sc_game_stop(void)
{
    running = 0;
}

void
sc_game_mainloop(void)
{
    running = 1;
    while (running)
        iterate_mainloop();
}

void
sc_game_init(void)
{
}

void
sc_game_shutdown(void)
{
}
