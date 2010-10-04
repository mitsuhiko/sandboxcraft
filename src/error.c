#include <stdarg.h>

#include "sc_error.h"
#include "sc_stringutils.h"

static sc_error_t *last_error;

static const char *error_names[] = {
    "out of memory",                                        /* SC_EOOM */
    "general error",                                        /* SC_GENERAL */
    "SDL error",                                            /* SC_ESDL */
    "Graphic subsystem error",                              /* SC_EGRAPHIC */
    "Shader subsystem error",                               /* SC_ESHADER */
    NULL
};

static void
show_error(int errno, const char *filename, int lineno, const char *msg)
{
    fprintf(stderr, "Error %d [%s]: %s", errno,
            error_names[errno], msg ? msg : "no further error description");
    if (filename) {
        fprintf(stderr, "Error location: %s", filename);
        if (lineno >= 0)
            fprintf(stderr, ":%d", lineno);
        fputs(stderr, "\n");
    }
}

void
sc_show_last_error(void)
{
    if (!last_error)
        return
    show_error(last_error->errno,
               last_error->filename,
               last_error->lineno,
               last_error->description);
}

void
sc_set_error(int errno, const char *filename, int lineno,
             const char *description, ...)
{
    va_list argptr;
    char *buf;
    va_start(argptr, description);
    vasprintf(&buf, description, argptr);
    va_end(argptr);

    if (!last_error) {
        last_error = sc_xalloc(sc_error_t);
        last_error->description = 0;
    }
    else {
        sc_free(last_error->description);
        sc_free(last_error->filename);
    }
    last_error->errno = errno;
    last_error->description = buf;
    last_error->filename = filename ? sc_strdup(filename) : NULL;
    last_error->lineno = lineno;
}

void
sc_clear_error(void)
{
    if (!last_error)
        return;
    /* description froms from vasprintf which might use malloc */
    free(last_error->description);
    sc_free(last_error);
}

void
sc_augment_error_context(const char *filename, int lineno)
{
    if (!last_error)
        return;
    sc_free(last_error->filename);
    last_error->filename = sc_strdup(filename);
    last_error->lineno = lineno;
}

void
sc_error_make_critical(void)
{
    if (!last_error)
        return;
    sc_show_last_error();
    exit(1);
}

void
sc_critical_error(int errno, const char *filename, int lineno,
                  const char *description, ...)
{
    char *buf;
    va_list argptr;
    va_start(argptr, description);
    vasprintf(&buf, description, argptr);
    va_end(argptr);
    show_error(errno, filename, lineno, buf);
    exit(1);
}

int
sc_has_error(void)
{
    return !!last_error;
}
