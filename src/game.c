#include "sc_game.h"
#include "sc_texture.h"
#include "sc_blocks.h"
#include "sc_world.h"
#include "sc_camera.h"
#include "sc_primitives.h"

static int running;
static int late_initialized;


static sc_world_t *world;
static sc_camera_t *cam;
struct {
    int w;
    int a;
    int s;
    int d;
} keysdown;


static void
perform_late_init(void)
{
    sc_texture_t *loading = sc_texture_from_resource("loading.png", GL_NEAREST);
    GLfloat vertices[16] = {
        -10.0f, -10.0f,  10.0f,
            10.0f, -10.0f,  10.0f,
            10.0f, -10.0f, -10.0f,
        -10.0f, -10.0f, -10.0f
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, sc_engine_get_aspect(), 1.0f, 200.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glLoadIdentity();
        glTranslatef(0.0f, 8.0f, -30.0f);
        glRotatef(35.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(25.0f, 0.0f, 1.0f, 0.0f);
        sc_bind_texture(loading);
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glDrawArrays(GL_QUADS, 0, 4);
    glPopMatrix();

    sc_engine_swap_buffers();
    sc_game_late_init();
    sc_free_texture(loading);
}

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
    if (evt->type == SDL_KEYDOWN)
        switch (evt->key.keysym.sym) {
        case SDLK_ESCAPE: sc_game_stop(); break;
        case SDLK_w: keysdown.w = 1; break;
        case SDLK_a: keysdown.a = 1; break;
        case SDLK_s: keysdown.s = 1; break;
        case SDLK_d: keysdown.d = 1; break;
        default:;
        }
    else if (evt->type == SDL_KEYUP)
        switch (evt->key.keysym.sym) {
        case SDLK_w: keysdown.w = 0; break;
        case SDLK_a: keysdown.a = 0; break;
        case SDLK_s: keysdown.s = 0; break;
        case SDLK_d: keysdown.d = 0; break;
        default:;
        }
    else if (evt->type == SDL_MOUSEMOTION)
        sc_camera_rotate_screen(cam, evt->motion.xrel * 0.25f,
                                evt->motion.yrel * 0.25f);
}

void
sc_game_update(void)
{
    float move_factor = sc_gametime.delta * 0.05f;
    if (keysdown.w)
        sc_camera_move_forward(cam, move_factor);
    if (keysdown.a)
        sc_camera_strafe_left(cam, move_factor);
    if (keysdown.s)
        sc_camera_move_backward(cam, move_factor);
    if (keysdown.d)
        sc_camera_strafe_right(cam, move_factor);
}

void
sc_game_render(void)
{
    sc_engine_clear(sc_color(0x24596Bff));
    sc_camera_apply(cam);
    sc_world_draw(world);
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
    while (running) {
        sc_engine_begin_frame();
        if (late_initialized) {
            sc_game_handle_events();
            sc_game_update();
            sc_game_render();
        }
        else
            perform_late_init();
        sc_engine_end_frame();
    }
}

void
sc_game_early_init(void)
{
    late_initialized = 0;
}

void
sc_game_late_init(void)
{
    int x, y, z;
    if (late_initialized)
        return;
    sc_init_blocks();

    world = sc_new_world();
    cam = sc_new_camera();
    sc_camera_set_position(cam, 0.0f, 40.0f, 40.0f);
    sc_camera_look_at(cam, 0.0f, 0.0f, 0.0f);
    sc_engine_grab_mouse(1);

    for (z = 0; z < SC_CHUNK_RESOLUTION; z++)
        for (y = 0; y < SC_CHUNK_RESOLUTION; y++)
            for (x = 0; x < SC_CHUNK_RESOLUTION; x++)
                sc_world_set_block(world, x, y, z,
                    z == SC_CHUNK_RESOLUTION -1
                        ? sc_get_block(SC_BLOCK_GRASS)
                        : sc_get_block(SC_BLOCK_STONE));

    sc_world_set_block(world, 5, 3, SC_CHUNK_RESOLUTION - 1, sc_get_block(SC_BLOCK_PLANKS));
    sc_world_set_block(world, 6, 3, SC_CHUNK_RESOLUTION - 1, sc_get_block(SC_BLOCK_PLANKS));
    sc_world_set_block(world, 6, 4, SC_CHUNK_RESOLUTION - 1, sc_get_block(SC_BLOCK_PLANKS));
    sc_world_set_block(world, 5, 4, SC_CHUNK_RESOLUTION - 1, sc_get_block(SC_BLOCK_PLANKS));

    late_initialized = 1;
}

void
sc_game_shutdown(void)
{
    sc_engine_grab_mouse(0);
    sc_free_camera(cam);
    sc_free_world(world);
    sc_free_blocks();
}
