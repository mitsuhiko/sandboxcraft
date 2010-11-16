#include "sc_boot.h"
#include "sc_pputils.h"

/* ensure that the includer set some types */
#if !defined(_SC_LIST_TYPE) || !defined(_SC_LIST_NAME)
#  error "Includer has to set _SC_LIST_TYPE and _SC_LIST_NAME"
#endif

/* helper macros to declare types and methods */
#define _SC_LIST_PREFIX SC_PP_UCAT(sc, _SC_LIST_NAME)
#define _SC_LIST_TYPENAME SC_PP_UCAT(_SC_LIST_PREFIX, t)
#define _SC_LIST_METHOD(Rv, Name) Rv SC_PP_UCAT(_SC_LIST_PREFIX, Name)
