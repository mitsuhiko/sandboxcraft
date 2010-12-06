/* string buffer type.  Very simple helper class for appening new strings
   on a dynamically growing buffer. */
#ifndef _INC_SC_STRBUF_H_
#define _INC_SC_STRBUF_H_

#include "sc_boot.h"

struct _sc_strbuf;
typedef struct _sc_strbuf sc_strbuf_t;

/* creates a new string buffer */
sc_strbuf_t *sc_new_strbuf(void);

/* frees the string buffer */
void sc_free_strbuf(sc_strbuf_t *strbuf);

/* appends a new c-string to the buffer */
void sc_strbuf_append(sc_strbuf_t *strbuf, const char *str);

/* appends a new c-string to the buffer with formatting */
void sc_strbuf_appendf(sc_strbuf_t *strbuf, const char *fmt, ...);

/* appends a character to the buffer */
void sc_strbuf_appendc(sc_strbuf_t *strbuf, char c);

/* appends arbitrary bytes to the buffer */
void sc_strbuf_append_bytes(sc_strbuf_t *strbuf, const char *bytes, size_t cnt);

/* returns the current size of the string buffer */
size_t sc_strbuf_size(const sc_strbuf_t *strbuf);

/* returns the buffer of the strbuf, optionally the number of bytes in the
   buffer.  After calling this you can further append stuff to the buffer
   but the pointer returned by this function should be seen as invalid.
   Also after this is called the strbuf object will no longer attempt to
   free the buffer upon destruction.  The buffer will always be zero
   terminated but might have zeros before the closing one. */
char *sc_strbuf_get_contents(sc_strbuf_t *strbuf, size_t *size_out);

/* resets the released flag.  This can be called after sc_strbuf_get_contents
   was called to invalidate the pointer and release the buffer's contents
   together with the call to sc_free_strbuf again. */
void sc_strbuf_invalidate_contents(sc_strbuf_t *strbuf);

/* shortcut for free and return buffer */
char *sc_free_strbuf_and_get_contents(sc_strbuf_t *strbuf, size_t *size_out);

#endif
