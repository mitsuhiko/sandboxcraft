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

void *sc_memassert(void *p);

#endif
