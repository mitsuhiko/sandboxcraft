/* simple growing list implemented as array */
#ifndef _INC_SC_LIST_H_
#define _INC_SC_LIST_H_

#include "sc_boot.h"

/* list of pointers */
#define _SC_LIST_TYPE void *
#define _SC_LIST_NAME list
#include "_sc_list.h"

/* list of integers */
#define _SC_LIST_TYPE int
#define _SC_LIST_NAME intlist
#include "_sc_list.h"

/* list of floats */
#define _SC_LIST_TYPE float
#define _SC_LIST_NAME floatlist
#include "_sc_list.h"

#endif
