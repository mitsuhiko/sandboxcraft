#include "sc_list.h"

/* list of pointers */
#define _SC_LIST_TYPE void *
#define _SC_LIST_NAME list
#include "_list_impl.inc"

/* list of integers */
#define _SC_LIST_TYPE int
#define _SC_LIST_NAME intlist
#include "_list_impl.inc"

/* list of floats */
#define _SC_LIST_TYPE float
#define _SC_LIST_NAME floatlist
#include "_list_impl.inc"
