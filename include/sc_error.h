/* implements a very basic error signalling system.  This is currently not
   thread safe which limits the whole application to single threaded
   execution but that's fine for the time being. */
#ifndef _INC_SC_ERROR_H_
#define _INC_SC_ERROR_H_

#include "sc_boot.h"

typedef struct {
    int errno;
    char *description;
} sc_error_t;

/* shows the given error on stderr */
void sc_show_error(int errno, const char *description);

/* shows the last set error */
void sc_show_last_error(void);

/* sets an error */
void sc_set_error(int errno, const char *description);

/* shows an error and aborts */
void sc_critical_error(int errno, const char *description);

/* true if there is an error */
int sc_has_error(void);

/* unsets an error again */
void sc_unset_error(void);

/* error codes, must correspond to the names in error.c */
#define SC_EOOM             0
#define SC_EGENERAL         1
#define SC_ESDL             2
#define SC_EGRAPHIC         3

#endif
