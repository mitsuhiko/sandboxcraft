#include "sc_alloc.h"
#include "sc_error.h"

void
sc_stackalloc_init(sc_stackalloc_t *alloc, void *mem, size_t size)
{
    alloc->mem = mem;
    alloc->size = size;
    alloc->used = 0;
}

void *
sc_stackalloc_alloc(sc_stackalloc_t *alloc, size_t size)
{
    void *rv;
    if (alloc->used + size > alloc->size) {
        sc_set_error(SC_EOOM, NULL, 0, "out of memory on stack allocation");
        return NULL;
    }
    rv = alloc->mem + alloc->used;
    alloc->used += size;
    return rv;
}

void *
sc_memassert(void *p)
{
    if (!p) {
        sc_critical_error(SC_EOOM, NULL, 0, "out of memory on general allocation");
        exit(1);
    }
    return p;
}
