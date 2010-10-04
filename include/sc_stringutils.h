/* string utilities */
#ifndef _INC_SC_STRINGUTILS_H_
#define _INC_SC_STRINGUTILS_H_

#include "sc_boot.h"

/* just in case.  Beware when switching out the allocator */
#if SC_PLATFORM == SC_PLATFORM_WINDOWS
#   define sc_strdup _strdup
#else
#   define sc_strdup strdup
#endif

#endif
