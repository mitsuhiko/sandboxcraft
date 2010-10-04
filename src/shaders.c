#include <stdio.h>
#include <string.h>

#include "sc_shaders.h"
#include "sc_path.h"
#include "sc_error.h"

static char *
next_token(const char *str, void **ctx)
{
    const char *rv_start;
    size_t size = 0;
    char *rv = NULL;
    str = *(const char **)ctx ? *(const char **)ctx : str;

    while (*str && isspace(*str))
        str++;
    rv_start = str;

    if (*str == '"' || *str == '\'') {
        char delim = *str++;
        while (*str != delim) {
            str++, size++;
            if (!*str)
                sc_critical_error(SC_ESHADER, "Unexpected end of string");
        }
        str++;
        size += 2;
    }
    else {
        while (*str && !isspace(*str))
            size++, str++;
        if (!size)
            return NULL;
    }

    rv = sc_xmalloc(size + 1);
    memcpy(rv, rv_start, size);
    rv[size] = '\0';
    *(const char **)ctx = str;
    return rv;
}

static int
end_of_parseline(void **ctx)
{
    const char *ptr = *(const char **)ctx;
    return ptr && (*ptr == '\n' || !*ptr);
}

static void
parse_shader(sc_program_t *program, GLuint shader_type,
              const char *line, void **tokenctx)
{
    char *filename = next_token(line, tokenctx);
    if (!filename)
        sc_critical_error(SC_ESHADER, "Shader name missing in program");
}

static void
parse_texture(sc_program_t *program, const char *line, void **tokenctx)
{
    char *unit = next_token(line, tokenctx);
    char *filename = next_token(line, tokenctx);
    sc_free(unit);
    sc_free(filename);
}

static void
parse_uniform(sc_program_t *program, const char *line, void **tokenctx)
{
    char *name = next_token(line, tokenctx);
    char *value = next_token(line, tokenctx);
    sc_free(name);
    sc_free(value);
}

static void
parse_program(sc_program_t *program, FILE *file)
{
    char line[2048];

    while (fgets(line, 2048, file)) {
        void *tokenctx = NULL;
        char *command = next_token(line, &tokenctx);
        if (!command || command[0] == '#')
            goto skip;
        else if (strcmp(command, "vertex") == 0)
            parse_shader(program, GL_VERTEX_SHADER, line, &tokenctx);
        else if (strcmp(command, "fragment") == 0)
            parse_shader(program, GL_FRAGMENT_SHADER, line, &tokenctx);
        else if (strcmp(command, "texture") == 0)
            parse_texture(program, line, &tokenctx);
        else if (strcmp(command, "uniform") == 0)
            parse_uniform(program, line, &tokenctx);
        else
            sc_critical_error(SC_ESHADER, "Unknown command in shader program");
        if (!end_of_parseline(&tokenctx))
            sc_critical_error(SC_ESHADER, "Trash after end of line");
skip:
        sc_free(command);
    }
}

sc_program_t *
sc_program_from_resource(const char *filename)
{
    char *path = sc_path_to_resource("shaders", filename);
    FILE *file = fopen(path, "r");
    if (!file)
        sc_critical_error(SC_ESHADER, "Unable to open shader program");

    sc_program_t *program = sc_xalloc(sc_program_t);
    program->shaders = NULL;
    program->textures = NULL;
    program->uniforms = NULL;

    parse_program(program, file);
    sc_free(path);
    fclose(file);
    return program;
}

void sc_free_program(sc_program_t *program)
{
    if (!program)
        return;
    sc_shaderid_t *iter = program->shaders;
    while (iter) {
        glDeleteShader(*iter);
        iter++;
    }
    if (program->id)
    glDeleteProgram(program->id);
    sc_free(program->shaders);
    sc_free(program);
}
