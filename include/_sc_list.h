#include "_sc_list_pre.inc"

#include "sc_utils.h"

typedef struct {
    size_t size;
    size_t allocated;
    _SC_LIST_TYPE *items;
} _SC_LIST_TYPENAME;

/* creates a new list */
_SC_LIST_TYPENAME *_SC_LIST_NEW_METHOD(size_t initial_size);

/* frees the list */
void _SC_LIST_FREE_METHOD(_SC_LIST_TYPENAME *list);

/* adds a new item to the list */
void _SC_LIST_METHOD(append)(_SC_LIST_TYPENAME *list, _SC_LIST_TYPE item);

/* removes an item from the list */
int _SC_LIST_METHOD(remove)(_SC_LIST_TYPENAME *list, _SC_LIST_TYPE item);

/* removes an item by index from the list */
void _SC_LIST_METHOD(delete)(_SC_LIST_TYPENAME *list, size_t idx);

/* removes the last item and returns it */
_SC_LIST_TYPE _SC_LIST_METHOD(pop)(_SC_LIST_TYPENAME *list);

/* gets an item from the list */
static sc_inline _SC_LIST_TYPE
_SC_LIST_METHOD(get)(_SC_LIST_TYPENAME *list, size_t idx)
{
    assert(idx < list->size);
    return list->items[idx];
}

/* finds an item in the list and returns the index or -1 if not found */
static sc_inline ssize_t
_SC_LIST_METHOD(find)(_SC_LIST_TYPENAME *list, _SC_LIST_TYPE item)
{
    ssize_t idx;
    for (idx = 0; idx < list->size; idx++)
        if (list->items[idx] == item)
            return idx;
    return -1;
}

/* trims the list (reallocates the list to strip) */
void _SC_LIST_METHOD(trim)(_SC_LIST_TYPENAME *list);

/* reverses a list in place */
void _SC_LIST_METHOD(reverse)(_SC_LIST_TYPENAME *list);

/* sort a list */
void _SC_LIST_METHOD(sort)(_SC_LIST_TYPENAME *list, sc_cmpfunc cmpfunc,
                           void *closure);

/* extends the list with the contents from another one */
void _SC_LIST_METHOD(extend)(_SC_LIST_TYPENAME *list,
                             _SC_LIST_TYPENAME *other_list);

/* creates a clone of the list */
_SC_LIST_TYPENAME *_SC_LIST_METHOD(clone)(_SC_LIST_TYPENAME *list);

#include "_sc_list_post.inc"
