/* utilities for path manipulation */
#ifndef _INC_SC_PATH_H_
#define _INC_SC_PATH_H_

#include "sc_boot.h"

const char *sc_get_resource_path(void);
char *sc_path_to_resource(const char *kind, const char *resource);

#endif
