/* helpers for memory allocation */
#ifndef _INC_SC_ALLOC_H_
#define _INC_SC_ALLOC_H_

#include "sc_boot.h"

/* basic allocation */
#define sc_malloc malloc
#define sc_free free
#define sc_calloc calloc
#define sc_alloc(t) sc_malloc(sizeof(t))
#define sc_realloc realloc

/* for unchecked allocation that directly terminates the application */
#define sc_xmalloc(p) sc_memassert(sc_malloc(p))
#define sc_xalloc(t) sc_xmalloc(sizeof(t))
#define sc_xcalloc(c, s) sc_memassert(sc_calloc(c, s))
#define sc_xrealloc(p, s) sc_memassert(sc_realloc(p, s))

/* stack allocation helper.  This allocates some memory on a specific
   memory location (can be heap or stack) and will then internally build
   up a stack for memory.  From that managed memory one can then allocate
   new memory and dynamically fall back to malloc or failing with an error */
typedef struct {
    void *mem;          /* the real memory location */
    size_t size;        /* size of stack in bytes */
    size_t used;        /* bytes used */
} sc_stackalloc_t;

/* initializes the stack allocator with given memory */
void sc_stackalloc_init(sc_stackalloc_t *alloc, void *mem, size_t size);

/* allocates memory on the stack and returns it.  If it ran out of memory on
   the stack, NULL is returned and an error is set.  This is unlike the other
   memory allocation functions which usually fail with a fatal error.
   Reasoning is that it's very likely to run out of this memory and you might
   want to retry fetching memory from somewhere else */
void *sc_stackalloc_alloc(sc_stackalloc_t *alloc, size_t size);

/* simple test if a pointer is managed by the stack allocator or if it came
   from somewhere else. */
static sc_inline int
sc_stackalloc_managed(const sc_stackalloc_t *alloc, void *ptr)
{
    return ptr >= alloc->mem && ptr < (alloc->mem + alloc->size);
}

/* sets a critical memory error if p is NULL */
void *sc_memassert(void *p);

#endif
