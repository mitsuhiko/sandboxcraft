/* preprocessor utilities */
#ifndef _INC_SC_PPUTILS_H_
#define _INC_SC_PPUTILS_H_

#define _SC_PP_CAT(A, B) A##B
#define _SC_PP_TOSTRING(A) #A

/* concatenates two tokens */
#define SC_PP_CAT(A, B) _SC_PP_CAT(A, B)

/* concatenates two tokens with an underline */
#define SC_PP_UCAT(A, B) SC_PP_CAT(SC_PP_CAT(A, _), B)

/* converts a token into a constant string */
#define SC_PP_TOSTRING(A) _SC_PP_TOSTRING(A)

#endif
