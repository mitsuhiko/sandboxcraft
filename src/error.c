#include <stdarg.h>

#include "sc_error.h"
#include "sc_utils.h"

/* XXX: make this thread local */
static sc_error_t *last_error = 0;

static const char *error_names[] = {
    "out of memory",                                        /* SC_EOOM */
    "general error",                                        /* SC_GENERAL */
    "SDL error",                                            /* SC_ESDL */
    "Graphic subsystem error",                              /* SC_EGRAPHIC */
    "Threading error",                                      /* SC_ETHREAD */
    "Shading system error",                                 /* SC_ESHADING */
    "Missing entity",                                       /* SC_ENOENT */
    "Input/Output error",                                   /* SC_EIO */
    NULL
};

static void
show_error(int code, const char *filename, int lineno, const char *msg)
{
#if SC_PLATFORM == SC_PLATFORM_WINDOWS
    /* maximum filename length on windows is 255, we allocate much more than that */
    char title[1024], *text;
    sprintf(title, "Error %d [%s]", code, error_names[code]);
    text = sc_malloc(strlen(msg) + 4096);
    sprintf(text, "%s\n\nproblem location: %s:%d\n", msg,
            filename ? filename : "Unknown file", lineno);
    MessageBox(NULL, text, title, MB_OK);
    sc_free(text);
#else
    fprintf(stderr, "Error %d [%s]: %s\n", code,
            error_names[code], msg ? msg : "no further error description");
    if (filename) {
        fprintf(stderr, "  problem location: %s", filename);
        if (lineno > 0)
            fprintf(stderr, ":%d", lineno);
        fputs("\n", stderr);
    }
    fflush(stderr);
#endif
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
    buf = sc_vasprintf(argptr, description);
    va_end(argptr);

    if (!last_error) {
        last_error = sc_xalloc(sc_error_t);
    } else {
        sc_free(last_error->description);
        sc_free(last_error->filename);
    }
    last_error->code = code;
    last_error->description = buf;
    last_error->filename = filename ? sc_safe_strdup(filename) : NULL;
    last_error->lineno = lineno;
}

int
sc_get_errno(void)
{
    return last_error->code;
}

void
sc_clear_error(void)
{
    if (!last_error)
        return;
    /* description comes from vasprintf which might use malloc */
    free(last_error->description);
    free(last_error->filename);
    sc_free(last_error);
    last_error = NULL;
}

void
sc_augment_error_context(const char *filename, int lineno)
{
    if (!last_error)
        return;
    sc_free(last_error->filename);
    last_error->filename = sc_safe_strdup(filename);
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
    buf = sc_vasprintf(argptr, description);
    va_end(argptr);
    show_error(code, filename, lineno, buf);
    exit(1);
}

int
sc_has_error(void)
{
    return !!last_error;
}
