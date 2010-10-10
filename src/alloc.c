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
