/* utilities for path manipulation */
#ifndef _INC_SC_PATH_H_
#define _INC_SC_PATH_H_

#include "sc_boot.h"

/* returns the path to the resource folder */
const char *sc_get_resource_path(void);

/* returns the path to a specific resource of a specific kind.
   The kind corresponds to the folder the resource is located in.
   If subfolders are used, slashes must be used to access a
   resource therein.  The return value must be sc_free()ed */
char *sc_path_to_resource(const char *kind, const char *resource);

#endif
