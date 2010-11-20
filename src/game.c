#include "sc_game.h"
#include "sc_texture.h"
#include "sc_blocks.h"
#include "sc_world.h"
#include "sc_camera.h"
#include "sc_primitives.h"
#include "sc_vbo.h"
#include "sc_worldgen.h"
#include "sc_threads.h"

static int running;

static sc_world_t *world;
static sc_camera_t *cam;
struct {
    int x;
    int y;
    int z;
    int side;
} selected_block;
struct {
    int w;
    int a;
    int s;
    int d;
} keysdown;


static int
init_game_in_thread(void *closure)
{
    int *done = closure;

    world = sc_create_random_world(256);
    cam = sc_new_camera();
    sc_camera_push(cam);
    sc_camera_set_position(cam, 0.0f, 40.0f, 40.0f);
    sc_camera_look_at(cam, 0.0f, 0.0f, 0.0f);
    sc_engine_grab_mouse(1);

    *done = 1;
    return 0;
}

static void
init_game(void)
{
    SDL_Event evt;
    int done = 0;
    float angle = 35.0f;
    sc_thread_t *load_thread;
    sc_texture_t *loading = sc_texture_from_resource("loading.png", GL_NEAREST);
    GLfloat vertices[16] = {
        -10.0f, -10.0f,  10.0f,
            10.0f, -10.0f,  10.0f,
            10.0f, -10.0f, -10.0f,
        -10.0f, -10.0f, -10.0f
    };

    /* this has to happen in the main thread before anything else */
    sc_init_blocks();

    load_thread = sc_new_thread(init_game_in_thread, &done);

    while (!done) {
        sc_engine_begin_frame();

        /* allow aborting the loading process */
        while (SDL_PollEvent(&evt))
            if (evt.type == SDL_QUIT ||
                (evt.type == SDL_KEYDOWN &&
                 evt.key.keysym.sym == SDLK_ESCAPE))
                exit(0);

        angle = (angle + sc_gametime.delta * 0.05f);
        if (angle > 360.0f)
            angle = 0.0f;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, sc_engine_get_aspect(), 1.0f, 200.0f);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            glTranslatef(0.0f, 8.0f, -30.0f);
            glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(25.0f, 0.0f, 1.0f, 0.0f);
            sc_bind_texture(loading);
            sc_send_texture_coordinates(loading);
            glRotatef(angle, 0.0f, 1.0f, 0.0f);
            glVertexPointer(3, GL_FLOAT, 0, vertices);
            glDrawArrays(GL_QUADS, 0, 4);
        glPopMatrix();
        sc_engine_end_frame();
    }

    /* this has to happen in the main thread after world was created */
    sc_world_flush_vbos(world);

    sc_free_texture(loading);
}

static void
shutdown_game(void)
{
    sc_engine_grab_mouse(0);
    sc_camera_pop();
    sc_free_camera(cam);
    sc_free_world(world);
    sc_free_blocks();
}

static void
select_center_block_side(void)
{
    sc_ray_t ray;
    ray.pos = cam->position;
    ray.dir = cam->forward;

    sc_world_raytest(world, &ray, &selected_block.x,
                     &selected_block.y, &selected_block.z,
                     &selected_block.side);
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
    else if (evt->type == SDL_MOUSEMOTION) {
        select_center_block_side();
        sc_camera_rotate_screen(cam, evt->motion.xrel * 0.25f,
                                evt->motion.yrel * 0.25f);
    }
    else if (evt->type == SDL_MOUSEBUTTONUP)
        sc_world_set_block(world, selected_block.x, selected_block.y,
                           selected_block.z, sc_get_block(SC_BLOCK_WATER));
}

void
sc_game_update(void)
{
    const float move_factor = sc_gametime.delta * 0.025f;

    /* camera movement */
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
    sc_engine_clear(sc_color(0x93ddefff));
    sc_apply_current_camera();
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
    init_game();
    while (running) {
        sc_engine_begin_frame();
        sc_game_handle_events();
        sc_game_update();
        sc_game_render();
        sc_engine_end_frame();
    }
    shutdown_game();
}
