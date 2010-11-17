/* simple growing list implemented as array.  This is implemented in a
   way that it's simple to generate a new implementation for a specific
   type.  The actual prototypes are in the "_sc_list.h" file.
   
   Some of the includes (pre/post) are also used in the implementation
   to set up and tear down the same infrastructure used in the headers. */
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
