/* maps string keys to pointers.  Implemented as binary search tree */
#ifndef _INC_SC_STRMAP_H_
#define _INC_SC_STRMAP_H_

#include "sc_boot.h"

struct _sc_strmap;
typedef struct _sc_strmap sc_strmap_t;

/* callback for iteration */
typedef void (*sc_strmap_callback)(const char *key, void *value, void *closure);

/* creates a new string map */
sc_strmap_t *sc_new_strmap(void);

/* frees the string map */
void sc_free_strmap(sc_strmap_t *strmap);

/* looks up a value by key.  If missing, NULL is returend */
void *sc_strmap_get(const sc_strmap_t *strmap, const char *key);

/* store a value in the strmap */
void sc_strmap_set(sc_strmap_t *strmap, const char *key, void *value);

/* removes an item from the strmap */
int sc_strmap_remove(sc_strmap_t *strmap, const char *key);

/* return the size of the strmap */
size_t sc_strmap_size(const sc_strmap_t *strmap);

/* calls a function for each pair in the map */
void sc_strmap_each(const sc_strmap_t *strmap, sc_strmap_callback cb, void *closure);

#endif
