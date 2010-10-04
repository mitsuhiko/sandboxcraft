#include "sc_game.h"
#include "sc_texture.h"
#include "sc_shaders.h"

static int running;

static sc_texture_t *grass_texture;


void
sc_game_handle_events(void)
{
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
        if (evt.type == SDL_QUIT)
            sc_game_stop();
        else
            sc_game_handle_event(&evt);
    }
}

void
sc_game_handle_event(SDL_Event *evt)
{
    if (evt->type == SDL_KEYDOWN &&
        evt->key.keysym.sym == SDLK_ESCAPE)
        sc_game_stop();
}

void
sc_game_update(void)
{
}

void
sc_game_render(void)
{
    /* clear display first and load identity matrix into model view */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* rotate and translate camera */
    glTranslatef(0.0f, 0.0f, -50.0f);
    glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(45.0f, 0.0f, 1.0f, 0.0f);

    /* draw a tile */
    GLfloat vertices[16] = {
        -10.0f, -10.0f,  10.0f,
         10.0f, -10.0f,  10.0f,
         10.0f, -10.0f, -10.0f,
        -10.0f, -10.0f, -10.0f
    };
    sc_bind_texture(grass_texture);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_QUADS, 0, 4);
}

void
sc_game_stop(void)
{
    running = 0;
}

void
sc_game_mainloop(void)
{
    grass_texture = sc_texture_from_resource("grass.png", GL_NEAREST);

    running = 1;
    while (running) {
        sc_engine_begin_frame();
        sc_game_handle_events();
        sc_game_update();
        sc_game_render();
        sc_engine_end_frame();
    }

    sc_free_texture(grass_texture);
}

void
sc_game_init(void)
{
    sc_program_t *program = sc_program_from_resource("grass.program");
    sc_free_program(program);
}

void
sc_game_shutdown(void)
{
}
