#include "sc_engine.h"
#include "sc_vec4.h"
#include "sc_error.h"
#include "sc_rnd.h"
#include "sc_debug.h"

/* TODO: move to config */
#define WIDTH 800
#define HEIGHT 600
#define FULLSCREEN 0
#define FPS_LIMIT 60

sc_gametime_t sc_gametime;
static int mouse_grabbed;
static int matrix_cache_dirty;
static sc_mat4_t view_matrix;
static sc_mat4_t model_matrix;
static sc_mat4_t modelview_matrix;
static sc_mat4_t projection_matrix;
static sc_mat4_t mvp_matrix;
static sc_mat3_t normal_matrix;

static struct {
    sc_ticks_t last_reset;
    size_t frames;
    size_t avg;
} fps_info;

static void
resize_viewport(void)
{
    sc_mat4_t identity;
    Uint32 flags = SDL_OPENGL;
    if (FULLSCREEN)
        flags |= SDL_FULLSCREEN;
    if (!SDL_SetVideoMode(WIDTH, HEIGHT, 32, flags))
        sc_critical_error(SC_ESDL, __FILE__, __LINE__, "%s", SDL_GetError());

    SDL_WM_SetCaption("SandboxCraft", NULL);

    /* activate opengl features */
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    /* do backface culling */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    /* setup viewport */
    glViewport(0, 0, WIDTH, HEIGHT);

    /* matrix defaults */
    sc_mat4_set_identity(&identity);
    model_matrix = identity;
    view_matrix = identity;
    projection_matrix = identity;
    matrix_cache_dirty = 1;
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

void
sc_engine_init(void)
{
    Uint32 sdl_flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
    if (SDL_Init(sdl_flags) < 0)
        sc_critical_error(SC_ESDL, __FILE__, __LINE__, "%s", SDL_GetError());

    sc_init_default_rnd();

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
sc_engine_get_dimensions(int *width, int *height)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    assert(viewport[0] == 0 && viewport[1] == 0);
    *width = viewport[2];
    *height = viewport[3];
}

float
sc_engine_get_aspect(void)
{
    int width, height;
    sc_engine_get_dimensions(&width, &height);
    return (float)width / height;
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
    } else {
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

static void
update_cached_matrixes(void)
{
    sc_mat4_t mat;
    if (!matrix_cache_dirty)
        return;
    sc_mat4_mul(&modelview_matrix, &model_matrix, &view_matrix);
    sc_mat4_mul(&mvp_matrix, &modelview_matrix, &projection_matrix);
    sc_mat4_inverse(&mat, &model_matrix);
    sc_mat4_transpose(&mat, &mat);
    sc_mat3_from_mat4(&normal_matrix, &mat);
    matrix_cache_dirty = 0;
}

sc_mat4_t *
sc_engine_get_view_matrix(sc_mat4_t *mat_out)
{
    *mat_out = view_matrix;
    return mat_out;
}

void
sc_engine_set_view_matrix(const sc_mat4_t *mat)
{
    view_matrix = *mat;
    matrix_cache_dirty = 1;
}

sc_mat4_t *
sc_engine_get_model_matrix(sc_mat4_t *mat_out)
{
    *mat_out = model_matrix;
    return mat_out;
}

void
sc_engine_set_model_matrix(const sc_mat4_t *mat)
{
    model_matrix = *mat;
    matrix_cache_dirty = 1;
}

sc_mat4_t *
sc_engine_get_projection_matrix(sc_mat4_t *mat_out)
{
    *mat_out = projection_matrix;
    return mat_out;
}

void
sc_engine_set_projection_matrix(const sc_mat4_t *mat)
{
    projection_matrix = *mat;
    matrix_cache_dirty = 1;
}

sc_mat4_t *
sc_engine_get_modelview_matrix(sc_mat4_t *mat_out)
{
    update_cached_matrixes();
    *mat_out = modelview_matrix;
    return mat_out;
}

sc_mat4_t *
sc_engine_get_mvp_matrix(sc_mat4_t *mat_out)
{
    update_cached_matrixes();
    *mat_out = mvp_matrix;
    return mat_out;
}

sc_mat3_t *
sc_engine_get_normal_matrix(sc_mat3_t *mat_out)
{
    update_cached_matrixes();
    *mat_out = normal_matrix;
    return mat_out;
}

sc_vec3_t *
sc_engine_unproject(sc_vec3_t *vec_out, int x, int y)
{
    int width, height;
    GLfloat winz;
    sc_mat4_t mvp;
    sc_vec4_t invec;

    sc_engine_get_mvp_matrix(&mvp);
    if (!sc_mat4_inverse(&mvp, &mvp))
        return NULL;

    sc_engine_get_dimensions(&width, &height);
    glReadPixels(x, height - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winz);

    invec.x = x * 2.0f / width - 1.0f;
    invec.y = (height - y) * 2.0f / height - 1.0f;
    invec.z = winz * 2.0f - 1.0f;
    invec.w = 1.0f;

    return sc_vec4_transform_homogenous(vec_out, &invec, &mvp);
}

sc_ray_t *
sc_engine_raycast(sc_ray_t *ray_out, int x, int y)
{
    int width, height;
    float cx, cy;
    sc_mat4_t mvp;
    sc_vec4_t vec1, vec2;

    sc_engine_get_mvp_matrix(&mvp);
    if (!sc_mat4_inverse(&mvp, &mvp))
        return NULL;

    sc_engine_get_dimensions(&width, &height);

    cx = x * 2.0f / width - 1.0f;
    cy = (height - y) * 2.0f / height - 1.0f;
    sc_vec4_set(&vec1, cx, cy, 0.0f, 1.0f);
    sc_vec4_set(&vec2, cx, cy, 1.0f, 1.0f);

    if (!sc_vec4_transform_homogenous(&ray_out->pos, &vec1, &mvp) ||
        !sc_vec4_transform_homogenous(&ray_out->dir, &vec2, &mvp))
        return NULL;

    sc_vec3_normalize(&ray_out->dir, &ray_out->dir);

    return ray_out;
}

void
sc_engine_begin_frame(void)
{
    sc_gametime.start = sc_engine_get_ticks();
}

void
sc_engine_end_frame(void)
{
    /* print debugging information if not in production mode */
#ifndef NDEBUG
    sc_debug_flush();
#endif

    /* this is not vsync, this is the limit we need for the update loop
       or a fast machine will be faster than the SDL timer resolution */
    if (sc_gametime.delta < (1000.0f / FPS_LIMIT))
        sc_engine_delay((sc_ticks_t)((1000.0f / FPS_LIMIT) -
                                     sc_gametime.delta));

    sc_gametime.end = sc_engine_get_ticks();
    sc_gametime.delta = sc_gametime.end - sc_gametime.start;

    /* do fps counting */
    fps_info.frames++;
    if (fps_info.last_reset < sc_gametime.end - 1000) {
        fps_info.avg = fps_info.frames;
        fps_info.frames = 0;
        fps_info.last_reset = sc_gametime.end;
    }

    sc_engine_swap_buffers();
}
