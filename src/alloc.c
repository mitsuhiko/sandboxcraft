#include "sc_alloc.h"
#include "sc_error.h"

void *
sc_memassert(void *p)
{
    if (!p) {
        sc_critical_error(SC_EOOM, NULL, 0, "out of memory on general allocation");
        exit(1);
    }
    return p;
}

void *
sc_xmalloc(size_t size)
{
    return sc_memassert(sc_malloc(size));
}

void *
sc_xcalloc(size_t count, size_t size)
{
    return sc_memassert(sc_calloc(count, size));
}
