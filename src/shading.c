#include "sc_shading.h"
#include "sc_path.h"
#include "sc_list.h"

#define ASSERT_FINALIZED(Shader) \
    assert((Shader)->program_id != 0)
#define ASSERT_NOT_FINALIZED(Shader) \
    assert((Shader)->program_id == 0)

struct _sc_shader {
    GLuint program_id;
    sc_intlist_t *modules;
};

static GLenum
convert_shader_type(sc_shadertype_t type)
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
    if (!sc_shader_finalize(rv))
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

int
sc_shader_attach_from_file(sc_shader_t *shader, const char *filename,
                           sc_shadertype_t type)
{
    int success;
    FILE *file;
    char *path;
    char *source;
    size_t end;
    GLuint shader_id;
    ASSERT_NOT_FINALIZED(shader);

    path = sc_path_to_resource("shaders", filename);
    file = fopen(path, "r");
    if (!file) {
        sc_set_error(SC_ENOENT, path, 0, "Unable to load shader");
        sc_free(path);
        return 0;
    }

    fseek(file, 0, 2);
    end = ftell(file);
    fseek(file, 0, 0);
    source = sc_xmalloc(end + 1);
    fread(source, 1, end, file);
    source[end] = 0;

    shader_id = glCreateShader(convert_shader_type(type));
    glShaderSource(shader_id, 1, (const GLchar **)&source, NULL);
    sc_free(source);
    fclose(file);

    glCompileShader(shader_id);
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        int log_size;
        char *info_log;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_size);
        info_log = sc_xmalloc(log_size);
        glGetShaderInfoLog(shader_id, log_size, NULL, info_log);
        sc_set_error(SC_ESHADING, path, 0, "Unable to parse shader:\n%s",
                     info_log);
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
    ASSERT_FINALIZED(shader);
    glUseProgram(shader->program_id);
}

void
sc_shader_unbind(void)
{
    glUseProgram(0);
}
