/* helpers for memory allocation */
#ifndef _INC_SC_ALLOC_H_
#define _INC_SC_ALLOC_H_

#include "sc_boot.h"

/* basic allocation */
#define sc_malloc malloc
#define sc_free free
#define sc_calloc calloc
#define sc_alloc(t) sc_malloc(sizeof(t))
#define sc_xalloc(t) sc_xmalloc(sizeof(t))

/* for unchecked allocation that directly terminates the application */
void *sc_memassert(void *p);
void *sc_xmalloc(size_t size);
void *sc_xcalloc(size_t count, size_t size);

/* dependencies on switching out the allocator:
   1. sc_strdup is currently internally using the system's malloc */

#endif
