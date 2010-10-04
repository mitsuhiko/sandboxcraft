/* shader support */
#ifndef _INC_SC_SHADERS_H_
#define _INC_SC_SHADERS_H_

#include "sc_boot.h"
#include "sc_texture.h"

typedef GLuint sc_shaderid_t;
typedef GLuint sc_uniformid_t;

typedef struct {
    sc_uniformid_t id;
    union {
        GLfloat f[4];
        GLint i[4];
    } v;
} sc_uniform_t;

typedef struct {
    sc_shaderid_t id;
    sc_shaderid_t *shaders;
    sc_texture_t **textures;
    sc_uniform_t *uniforms;
} sc_program_t;

sc_program_t *sc_program_from_resource(const char *filename);
void sc_bind_program(sc_program_t *program);
void sc_free_program(sc_program_t *program);

#endif
