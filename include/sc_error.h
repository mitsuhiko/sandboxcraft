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
    int lineno;         /* if available, otherwise -1 */
} sc_error_t;

void sc_show_last_error(void);
void sc_set_error(int errno, const char *filename, int lineno,
                  const char *description, ...);
void sc_clear_error(void);
void sc_augment_error_context(const char *filename, int lineno);
void sc_error_make_critical(void);
void sc_critical_error(int errno, const char *filename, int lineno,
                       const char *description, ...);

/* error codes, must correspond to the names in error.c */
#define SC_EOOM             0
#define SC_EGENERAL         1
#define SC_ESDL             2
#define SC_EGRAPHIC         3
#define SC_ESHADER          4

#endif
