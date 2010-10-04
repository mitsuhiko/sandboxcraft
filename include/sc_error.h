/* implements a very basic error signalling system.  This is currently not
   thread safe which limits the whole application to single threaded
   execution but that's fine for the time being. */
#ifndef _INC_SC_ERROR_H_
#define _INC_SC_ERROR_H_

#include "sc_boot.h"

typedef struct {
    int errno;          /* internal error number, see below */
    char *description;  /* textual description of the error */
    char *filename;     /* the filename that caused that error.  This might
                           be a source file (__FILE__) or a shader file,
                           image file etc. */
    int lineno;         /* > 0 if available */
} sc_error_t;

/* shows the currently set error.  If there is no error, this does nothing */
void sc_show_last_error(void);

/* sets a new error.  This should be combined with special return values.
   The following rules are in place to signal errors:

   -    functions returning pointers must return NULL on error.
   -    functions returning signed integers must return -1 on
        error unless the return value is a boolean in which case
        0 should be assumed.
   -    functions returning unsigned integers must return 0 on
        error

   In case the return value clashes with a valid return value, the return
   value must be returned with a parameter. */
void sc_set_error(int errno, const char *filename, int lineno,
                  const char *description, ...);

/* clears the current error */
void sc_clear_error(void);

/* attaches context information to the current error if there is one */
void sc_augment_error_context(const char *filename, int lineno);

/* makes the last error critical (shows error and terminates the app) */
void sc_error_make_critical(void);

/* shortcut for set_error + make_critical */
void sc_critical_error(int errno, const char *filename, int lineno,
                       const char *description, ...);

/* error codes, must correspond to the names in error.c */
#define SC_EOOM             0
#define SC_EGENERAL         1
#define SC_ESDL             2
#define SC_EGRAPHIC         3

#endif
