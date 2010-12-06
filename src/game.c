#include "sc_game.h"
#include "sc_texture.h"
#include "sc_blocks.h"
#include "sc_world.h"
#include "sc_camera.h"
#include "sc_primitives.h"
#include "sc_vbo.h"
#include "sc_scenemgr.h"
#include "sc_shading.h"
#include "sc_worldgen.h"
#include "sc_threads.h"

static int running;

static sc_scenemgr_t *scenemgr;
static sc_shader_t *shader;
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
    sc_camera_t *cam;
    sc_light_t *light;
    int *done = closure;

    scenemgr = sc_new_scenemgr();
    sc_scenemgr_set_world(scenemgr, sc_create_random_world(256));
    cam = sc_scenemgr_get_active_camera(scenemgr);

    sc_camera_set_position(cam, 0.0f, 40.0f, 40.0f);
    sc_camera_look_at(cam, 0.0f, 0.0f, 0.0f);
    sc_engine_grab_mouse(1);

    light = sc_scenemgr_create_light(scenemgr);
    light->ambient = sc_color(0x444444ff);
    light->diffuse = sc_color(0xccccccff);
    light->specular = sc_color(0x888888ff);
    sc_vec4_set(&light->position, -1.5f, 1.0f, -4.0f, 1.0f);

    *done = 1;
    return 0;
}

static void
init_game(void)
{
    SDL_Event evt;
    int done = 0;
    float angle = 35.0f;
    sc_vbo_t *cube;
    sc_thread_t *load_thread;
    sc_texture_t *loading = sc_texture_from_resource("loading.png", GL_NEAREST);

    cube = sc_new_cube(10.0f);
    sc_vbo_finalize(cube, 0);

    /* this has to happen in the main thread before anything else */
    sc_init_blocks();
    shader = sc_shader_from_file("simple");

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
            glTranslatef(0.0f, 0.0f, -30.0f);
            glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(25.0f, 0.0f, 1.0f, 0.0f);
            glRotatef(angle, 0.0f, 1.0f, 0.0f);
            sc_bind_texture(loading);
            sc_vbo_draw(cube);
        glPopMatrix();
        sc_engine_end_frame();
    }

    /* this has to happen in the main thread after world was created */
    sc_world_flush_vbos(sc_scenemgr_get_world(scenemgr));

    sc_free_texture(loading);
    sc_free_vbo(cube);
}

static void
shutdown_game(void)
{
    sc_engine_grab_mouse(0);
    sc_free_world(sc_scenemgr_get_world(scenemgr));
    sc_free_scenemgr(scenemgr);
    sc_free_blocks();
    sc_free_shader(shader);
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
        sc_camera_t *cam = sc_scenemgr_get_active_camera(scenemgr);
        sc_camera_rotate_screen(cam, evt->motion.xrel * 0.25f,
                                evt->motion.yrel * 0.25f);
    }
}

void
sc_game_update(void)
{
    float move_factor = sc_gametime.delta * 0.025f;
    sc_camera_t *cam = sc_scenemgr_get_active_camera(scenemgr);

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
    sc_shader_bind(shader);
    sc_scenemgr_draw(scenemgr);
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
