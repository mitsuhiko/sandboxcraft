/* various utilities that could come in handy */
#ifndef _INC_SC_UTILS_H_
#define _INC_SC_UTILS_H_

#include "sc_boot.h"

/* string duplication function */
#if SC_PLATFORM == SC_PLATFORM_WINDOWS
#   define sc_strdup _strdup
#else
#   define sc_strdup strdup
#endif

/* min and max inline macro.  These have to be macros to support different
   types.  ALso they can't be implemented without side effects in a
   compiler independent way, so beware when using them. */
#define sc_min(a, b) ((a) < (b) ? (a) : (b))
#define sc_max(a, b) ((a) > (b) ? (a) : (b))

/* sort comparision function */
typedef int (*sc_cmpfunc)(const void *, const void *, void *);

/* generic reentrant sort function */
void sc_sort(void *base, size_t length, size_t size, void *closure,
             sc_cmpfunc cmpfunc);

#endif
