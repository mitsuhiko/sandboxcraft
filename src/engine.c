#include <assert.h>
#include <stdio.h>

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
    glEnable(GL_DEPTH_TEST);
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

    /* show detected features on stderr */
    sc_engine_dump_info();
}

void
sc_engine_shutdown(void)
{
    SDL_Quit();
}

void
sc_engine_dump_info(void)
{
    fprintf(stderr, "SDL Version:               %d.%d.%d\n",
            SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
    fprintf(stderr, "OpenGL Version:            %s\n",
            sc_engine_get_opengl_version());
    fprintf(stderr, "Vertex buffer support:     %s\n",
            GLEE_ARB_vertex_buffer_object ? "yes" : "no");
    fprintf(stderr, "Textures non power of two: %s\n",
            GLEE_ARB_texture_non_power_of_two ? "yes" : "no");
    fprintf(stderr, "ARB multisampling:         %s\n",
            GLEE_ARB_multisample ? "yes" : "no");
}

const char *
sc_engine_get_opengl_version(void)
{
    /* do not test for newer version as we're not supporting 3.0 */
    if (GLEE_VERSION_2_1) return "2.1";
    if (GLEE_VERSION_2_0) return "2.0";
    if (GLEE_VERSION_1_5) return "1.5";
    if (GLEE_VERSION_1_4) return "1.4";
    if (GLEE_VERSION_1_3) return "1.3";
    if (GLEE_VERSION_1_2) return "1.2";
    return "1.1";
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
sc_engine_clear(sc_color_t color)
{
    float r, g, b, a;
    sc_color_to_float(color, &r, &g, &b, &a);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

sc_mat4_t *
sc_engine_get_mvp_matrix(sc_mat4_t *mat_out)
{
    sc_mat4_t m, p;
    glGetFloatv(GL_MODELVIEW_MATRIX, m.elms);
    glGetFloatv(GL_PROJECTION_MATRIX, p.elms);
    return sc_mat4_mul(mat_out, &p, &m);
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
