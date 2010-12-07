#include "sc_shading.h"
#include "sc_engine.h"
#include "sc_path.h"
#include "sc_list.h"
#include "sc_strbuf.h"

#include "sc_vec2.h"
#include "sc_vec3.h"
#include "sc_vec4.h"
#include "sc_mat3.h"
#include "sc_mat4.h"

#define ASSERT_FINALIZED(Shader) \
    assert((Shader)->program_id != 0)
#define ASSERT_NOT_FINALIZED(Shader) \
    assert((Shader)->program_id == 0)

struct _sc_shader {
    GLuint program_id;
    sc_intlist_t *modules;
    sc_list_t *sources;
};

static const sc_shader_t *current_shader;

static GLenum
convert_shader_type(int type)
{
    switch (type) {
    case SC_VERTEX_SHADER:      return GL_VERTEX_SHADER;
    case SC_FRAGMENT_SHADER:    return GL_FRAGMENT_SHADER;
    default:                    assert(0); return 0;
    }
}


sc_shader_t *
sc_new_shader(void)
{
    sc_shader_t *rv = sc_alloc(sc_shader_t);
    rv->program_id = 0;
    rv->modules = sc_new_intlist();
    rv->sources = sc_new_list();
    return rv;
}

sc_shader_t *
sc_shader_from_file(const char *basename)
{
#define ATTACH(Suffix, Type) do { \
    size_t bn_len = strlen(basename); \
    size_t s_len = strlen(Suffix); \
    char *buf = sc_xmalloc(bn_len + s_len + 1); \
    strcpy(buf, basename); \
    strcpy(buf + bn_len, Suffix); \
    if (!sc_shader_attach_from_file(rv, buf, Type)) { \
        if (sc_get_errno() != SC_ENOENT) \
            goto error; \
        sc_clear_error(); \
    } \
} while (0)

    sc_shader_t *rv = sc_new_shader();
    ATTACH(".vert", SC_VERTEX_SHADER);
    ATTACH(".frag", SC_FRAGMENT_SHADER);
    if (sc_shader_finalize(rv))
        return rv;

error:
    sc_error_make_critical();
    return NULL;
}

void
sc_free_shader(sc_shader_t *shader)
{
    size_t i;
    if (!shader)
        return;
    if (shader->program_id)
        glDeleteProgram(shader->program_id);
    for (i = 0; i < shader->modules->size; i++)
        glDeleteShader((GLuint)shader->modules->items[i]);
    sc_free_intlist(shader->modules);
    sc_free(shader);
}

static char *
eat_string_literal(char *string)
{
    char *start, *ptr = string;
    while (*ptr == ' ' || *ptr == '\t')
        ptr++;
    if (*ptr++ != '"')
        return NULL;
    start = ptr;
    while (1) {
        if (!*ptr)
            return NULL;
        if (*ptr == '"')
            break;
        ptr++;
    }
    *ptr = '\0';
    return start;
}

static int
read_shader_source(sc_strbuf_t *strbuf, sc_shader_t *shader,
                   const char *filename, int type)
{
    size_t i;
    int source;
    int lineno = 1;
    char line[4096], *full_path, *include_file;
    FILE *file = fopen(filename, "r");
    int start_of_line = 1;
    if (!file) {
        sc_set_error(SC_ENOENT, filename, 0, "Unable to load shader");
        return 0;
    }

    for (i = 0; i < shader->sources->size; i++)
        if (strcmp(shader->sources->items[i], filename) == 0) {
            source = i;
            goto source_found;
        }
    source = shader->sources->size;
    sc_list_append(shader->sources, sc_strdup(filename));

source_found:
    if (type == SC_VERTEX_SHADER)
        sc_strbuf_appendf(strbuf, "#define SC_VERTEX_SHADER\n");
    else if (type == SC_FRAGMENT_SHADER)
        sc_strbuf_appendf(strbuf, "#define SC_FRAGMENT_SHADER\n");
    sc_strbuf_appendf(strbuf, "#line %d %d\n", 0, source);

    while (fgets(line, 4096, file)) {
        if (start_of_line && strncmp(line, "#include", 8) == 0) {
            include_file = eat_string_literal(line + 8);
            full_path = sc_path_join_with_dir(filename, include_file);
            if (!include_file) {
                sc_set_error(SC_ESHADING, filename, lineno,
                    "Invalid syntax for file include");
                return 0;
            }
            read_shader_source(strbuf, shader, full_path, 0);
            sc_strbuf_appendf(strbuf, "#line %d %d\n", lineno, source);
            sc_free(full_path);
        } else {
            sc_strbuf_append(strbuf, line);
        }
        start_of_line = line[strlen(line) - 1] == '\n';
        if (start_of_line)
            lineno++;
    }

    fclose(file);
    return 1;
}

static void
set_shading_error(const sc_shader_t *shader, const char *info_log,
                  const char *filename, const char *msg)
{
    size_t i;
    char *source_info;
    sc_strbuf_t *buf = sc_new_strbuf();
    for (i = 0; i < shader->sources->size; i++)
        sc_strbuf_appendf(buf, "  %d: %s\n", i, shader->sources->items[i]);
    source_info = sc_free_strbuf_and_get_contents(buf, NULL);
    sc_set_error(SC_ESHADING, filename, 0, "%s:\nSources:\n%s\n%s",
                 msg, source_info, info_log);
}

int
sc_shader_attach_from_file(sc_shader_t *shader, const char *filename,
                           int type)
{
    int success;
    char *path;
    char *source;
    sc_strbuf_t *strbuf = sc_new_strbuf();
    GLuint shader_id;
    ASSERT_NOT_FINALIZED(shader);

    path = sc_path_to_resource("shaders", filename);
    if (!read_shader_source(strbuf, shader, path, type)) {
        sc_free_strbuf(strbuf);
        sc_free(path);
        return 0;
    }

    source = sc_free_strbuf_and_get_contents(strbuf, NULL);
    shader_id = glCreateShader(convert_shader_type(type));
    glShaderSource(shader_id, 1, (const GLchar **)&source, NULL);
    sc_free(source);

    glCompileShader(shader_id);
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        int log_size;
        char *info_log;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_size);
        info_log = sc_xmalloc(log_size);
        glGetShaderInfoLog(shader_id, log_size, NULL, info_log);
        set_shading_error(shader, info_log, path, "Unable to parse shader");
        sc_free(info_log);
        sc_free(path);
        glDeleteShader(shader_id);
        return 0;
    }

    sc_intlist_append(shader->modules, (int)shader_id);
    sc_free(path);
    return 1;
}

int
sc_shader_finalize(sc_shader_t *shader)
{
    int success;
    size_t i;
    GLuint program_id;
    ASSERT_NOT_FINALIZED(shader);

    program_id = glCreateProgram();
    for (i = 0; i < shader->modules->size; i++)
        glAttachShader(program_id, shader->modules->items[i]);
    glLinkProgram(program_id);

    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        int log_size;
        char *info_log;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_size);
        info_log = sc_xmalloc(log_size);
        glGetProgramInfoLog(program_id, log_size, NULL, info_log);
        sc_set_error(SC_ESHADING, __FILE__, 0,
                     "Unable to link shader program:\n%s", info_log);
        sc_free(info_log);
        glDeleteProgram(program_id);
        return 0;
    }

    shader->program_id = program_id;
    return 1;
}

void
sc_shader_bind(const sc_shader_t *shader)
{
    sc_mat4_t mat4;
    sc_mat3_t mat3;
    ASSERT_FINALIZED(shader);

    /* together with the shader, bind common uniforms from common.shader */
    sc_engine_get_modelview_matrix(&mat4);
    sc_mat4_uniform(shader, "sc_modelview_matrix", &mat4);
    sc_engine_get_model_matrix(&mat4);
    sc_mat4_uniform(shader, "sc_model_matrix", &mat4);
    sc_engine_get_view_matrix(&mat4);
    sc_mat4_uniform(shader, "sc_view_matrix", &mat4);
    sc_engine_get_projection_matrix(&mat4);
    sc_mat4_uniform(shader, "sc_projection_matrix", &mat4);
    sc_engine_get_mvp_matrix(&mat4);
    sc_mat4_uniform(shader, "sc_mvp_matrix", &mat4);
    sc_engine_get_normal_matrix(&mat3);
    sc_mat3_uniform(shader, "sc_normal_matrix", &mat3);

    /* activate the shader */
    glUseProgram(shader->program_id);
    current_shader = shader;
}

void
sc_shader_unbind(void)
{
    current_shader = 0;
    glUseProgram(0);
}

const sc_shader_t *
sc_get_current_shader(void)
{
    return current_shader;
}

static int
get_uniform_location(const sc_shader_t *shader, const char *name, int check)
{
    int rv;
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    rv = glGetUniformLocation(shader->program_id, name);
    if (check && rv < 0)
        sc_critical_error(SC_ESHADING, NULL, 0,
            "Cannot find uniform %s", name);
    return rv;
}

static int
get_attrib_location(const sc_shader_t *shader, const char *name, int check)
{
    int rv;
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    rv = glGetAttribLocation(shader->program_id, name);
    if (check && rv < 0)
        sc_critical_error(SC_ESHADING, NULL, 0,
            "Cannot find vertex attribute %s", name);
    return rv;
}

void
sc_int_uniform(const sc_shader_t *shader, const char *name, int val)
{
    int loc = get_uniform_location(shader, name, 0);
    if (loc >= 0)
        glUniform1i(loc, val);
}

void
sc_float_uniform(const sc_shader_t *shader, const char *name, float val)
{
    int loc = get_uniform_location(shader, name, 0);
    if (loc >= 0)
        glUniform1f(loc, val);
}

void
sc_color_uniform(const sc_shader_t *shader, const char *name, sc_color_t val)
{
    float color[4];
    int loc = get_uniform_location(shader, name, 0);
    sc_color_to_floatv(val, color);
    if (loc >= 0)
        glUniform4fv(loc, 1, color);
}

void
sc_vec2_uniform(const sc_shader_t *shader, const char *name, const sc_vec2_t *vec)
{
    int loc = get_uniform_location(shader, name, 0);
    if (loc >= 0)
        glUniform2fv(loc, 1, sc_vec2_ptr(vec));
}

void
sc_vec3_uniform(const sc_shader_t *shader, const char *name, const sc_vec3_t *vec)
{
    int loc = get_uniform_location(shader, name, 0);
    if (loc >= 0)
        glUniform3fv(loc, 1, sc_vec3_ptr(vec));
}

void
sc_vec4_uniform(const sc_shader_t *shader, const char *name, const sc_vec4_t *vec)
{
    int loc = get_uniform_location(shader, name, 0);
    if (loc >= 0)
        glUniform4fv(loc, 1, sc_vec4_ptr(vec));
}

void
sc_mat3_uniform(const sc_shader_t *shader, const char *name, const struct _sc_mat3 *mat)
{
    int loc = get_uniform_location(shader, name, 0);
    if (loc >- 0)
        glUniformMatrix3fv(loc, 1, 0, sc_mat3_ptr(mat));
}

void
sc_mat4_uniform(const sc_shader_t *shader, const char *name, const struct _sc_mat4 *mat)
{
    int loc = get_uniform_location(shader, name, 0);
    if (loc >= 0)
        glUniformMatrix4fv(loc, 1, 0, sc_mat4_ptr(mat));
}

int
sc_get_int_uniform(const sc_shader_t *shader, const char *name)
{
    int rv;
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetUniformiv(shader->program_id, get_uniform_location(shader, name, 0), &rv);
    return rv;
}

float
sc_get_float_uniform(const sc_shader_t *shader, const char *name)
{
    float rv;
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetUniformfv(shader->program_id, get_uniform_location(shader, name, 0), &rv);
    return rv;
}

sc_color_t
sc_get_color_uniform(const sc_shader_t *shader, const char *name)
{
    float rv[4];
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetUniformfv(shader->program_id, get_uniform_location(shader, name, 1), rv);
    return sc_color_from_floatv(rv);
}

sc_vec2_t *
sc_get_vec2_uniform(const sc_shader_t *shader, const char *name, sc_vec2_t *vec_out)
{
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetUniformfv(shader->program_id, get_uniform_location(shader, name, 1),
                   sc_vec2_ptr(vec_out));
    return vec_out;
}

sc_vec3_t *
sc_get_vec3_uniform(const sc_shader_t *shader, const char *name, sc_vec3_t *vec_out)
{
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetUniformfv(shader->program_id, get_uniform_location(shader, name, 1),
                   sc_vec3_ptr(vec_out));
    return vec_out;
}

sc_vec4_t *
sc_get_vec4_uniform(const sc_shader_t *shader, const char *name, sc_vec4_t *vec_out)
{
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetUniformfv(shader->program_id, get_uniform_location(shader, name, 1),
                   sc_vec4_ptr(vec_out));
    return vec_out;
}

sc_mat3_t *
sc_get_mat3_uniform(const sc_shader_t *shader, const char *name, sc_mat3_t *mat_out)
{
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetUniformfv(shader->program_id, get_uniform_location(shader, name, 1),
                   sc_mat3_ptr(mat_out));
    return mat_out;
}

sc_mat4_t *
sc_get_mat4_uniform(const sc_shader_t *shader, const char *name, sc_mat4_t *mat_out)
{
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetUniformfv(shader->program_id, get_uniform_location(shader, name, 1),
                   sc_mat4_ptr(mat_out));
    return mat_out;
}

void
sc_float_attrib(const sc_shader_t *shader, const char *name, float val)
{
    int loc = get_attrib_location(shader, name, 0);
    if (loc >= 0)
        glVertexAttrib1f(loc, val);
}

void
sc_color_attrib(const sc_shader_t *shader, const char *name, sc_color_t val)
{
    float color[4];
    int loc = get_attrib_location(shader, name, 0);
    sc_color_to_floatv(val, color);
    if (loc >= 0)
        glVertexAttrib4fv(loc, color);
}

void
sc_vec2_attrib(const sc_shader_t *shader, const char *name, const sc_vec2_t *vec)
{
    int loc = get_attrib_location(shader, name, 0);
    if (loc >= 0)
        glVertexAttrib2fv(loc, sc_vec2_ptr(vec));
}

void
sc_vec3_attrib(const sc_shader_t *shader, const char *name, const sc_vec3_t *vec)
{
    int loc = get_attrib_location(shader, name, 0);
    if (loc >= 0)
        glVertexAttrib3fv(loc, sc_vec3_ptr(vec));
}

void
sc_vec4_attrib(const sc_shader_t *shader, const char *name, const sc_vec4_t *vec)
{
    int loc = get_attrib_location(shader, name, 0);
    if (loc >= 0)
        glVertexAttrib4fv(loc, sc_vec4_ptr(vec));
}

void
sc_floatb_attrib(const sc_shader_t *shader, const char *name, int size,
                 GLuint buffer)
{
    int loc = get_attrib_location(shader, name, 0);
    if (loc < 0)
        return;
    if (!buffer) {
        glDisableVertexAttribArray(loc);
        return;
    }
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(loc, size, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(loc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
sc_disable_buffer_attrib(const sc_shader_t *shader, const char *name)
{
    int loc = get_attrib_location(shader, name, 0);
    if (loc >= 0)
        glDisableVertexAttribArray(loc);
}

float
sc_get_float_attrib(const sc_shader_t *shader, const char *name)
{
    float rv;
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetVertexAttribfv(shader->program_id, get_attrib_location(shader, name, 1), &rv);
    return rv;
}

sc_color_t
sc_get_color_attrib(const sc_shader_t *shader, const char *name)
{
    float rv[4];
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetVertexAttribfv(shader->program_id, get_attrib_location(shader, name, 1), rv);
    return sc_color_from_floatv(rv);
}

sc_vec2_t *
sc_get_vec2_attrib(const sc_shader_t *shader, const char *name, sc_vec2_t *vec_out)
{
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetVertexAttribfv(shader->program_id, get_attrib_location(shader, name, 1), sc_vec2_ptr(vec_out));
    return vec_out;
}

sc_vec3_t *
sc_get_vec3_attrib(const sc_shader_t *shader, const char *name, sc_vec3_t *vec_out)
{
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetVertexAttribfv(shader->program_id, get_attrib_location(shader, name, 1),
                   sc_vec3_ptr(vec_out));
    return vec_out;
}

sc_vec4_t *
sc_get_vec4_attrib(const sc_shader_t *shader, const char *name, sc_vec4_t *vec_out)
{
    if (!shader) {
        shader = current_shader;
        assert(shader);
    }
    glGetVertexAttribfv(shader->program_id, get_attrib_location(shader, name, 1),
                   sc_vec4_ptr(vec_out));
    return vec_out;
}
