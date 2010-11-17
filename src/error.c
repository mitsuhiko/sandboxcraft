#include <stdarg.h>

#include "sc_error.h"
#include "sc_utils.h"

static sc_error_t *last_error;

static const char *error_names[] = {
    "out of memory",                                        /* SC_EOOM */
    "general error",                                        /* SC_GENERAL */
    "SDL error",                                            /* SC_ESDL */
    "Graphic subsystem error",                              /* SC_EGRAPHIC */
    NULL
};

static void
show_error(int code, const char *filename, int lineno, const char *msg)
{
    fprintf(stderr, "Error %d [%s]: %s\n", code,
            error_names[code], msg ? msg : "no further error description");
    if (filename) {
        fprintf(stderr, "  problem location: %s", filename);
        if (lineno > 0)
            fprintf(stderr, ":%d", lineno);
        fputs("\n", stderr);
    }
    fflush(stderr);
}

static char *
format_error(va_list argptr, const char *description)
{
#if SC_PLATFORM == SC_PLATFORM_WINDOWS
    size_t bufsize = _vscprintf(description, argptr) + 1;
    char *buf = (char *)malloc(bufsize);
    _vsnprintf(buf, bufsize, description, argptr);
#else
    char *buf;
    vasprintf(&buf, description, argptr);
#endif
    return buf;
}

void
sc_show_last_error(void)
{
    if (!last_error)
        return;
    show_error(last_error->code,
               last_error->filename,
               last_error->lineno,
               last_error->description);
}

void
sc_set_error(int code, const char *filename, int lineno,
             const char *description, ...)
{
    char *buf;
    va_list argptr;
    va_start(argptr, description);
    buf = format_error(argptr, description);
    va_end(argptr);

    if (!last_error) {
        last_error = sc_xalloc(sc_error_t);
    }
    else {
        sc_free(last_error->description);
        sc_free(last_error->filename);
    }
    last_error->code = code;
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
sc_critical_error(int code, const char *filename, int lineno,
                  const char *description, ...)
{
    char *buf;
    va_list argptr;
    va_start(argptr, description);
    buf = format_error(argptr, description);
    va_end(argptr);
    show_error(code, filename, lineno, buf);
    exit(1);
}

int
sc_has_error(void)
{
    return !!last_error;
}
