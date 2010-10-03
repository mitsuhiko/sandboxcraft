#include "sc_error.h"
#include "sc_stringutils.h"

static sc_error_t *last_error;

static const char *error_names[] = {
    "out of memory",                                        /* SC_EOOM */
    "general error",                                        /* SC_GENERAL */
    "SDL error",                                            /* SC_ESDL */
    NULL
};

void
sc_show_error(int errno, const char *description)
{
    fprintf(stderr, "Error %d [%s]: %s", errno,
            error_names[errno], description ? description :
                "no further error description");
}

void
sc_show_last_error(void)
{
    if (last_error)
        sc_show_error(last_error->errno, last_error->description);
}

void
sc_set_error(int errno, const char *description)
{
    if (!last_error) {
        last_error = sc_xalloc(sc_error_t);
        last_error->description = 0;
    }
    last_error->errno = errno;
    sc_free(last_error->description);
    /* let strdup fail.  if it's NULL we just don't display something */
    last_error->description = sc_strdup(description);
}

void
sc_critical_error(int errno, const char *description)
{
    sc_show_error(errno, description);
    exit(1);
}

int
sc_has_error(void)
{
    return !!last_error;
}

void
sc_unset_error(void)
{
    if (!last_error)
        return;
    sc_free(last_error->description);
    sc_free(last_error);
}
