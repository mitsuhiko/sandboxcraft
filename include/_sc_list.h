#include "_sc_list_pre.inc"

typedef struct {
    size_t size;
    size_t allocated;
    _SC_LIST_TYPE items[0];
} _SC_LIST_TYPENAME;

/* creates a new list */
_SC_LIST_METHOD(_SC_LIST_TYPENAME *, new)(size_t initial_size);

/* adds a new item to the list */
_SC_LIST_METHOD(void, append)(_SC_LIST_TYPENAME *list, _SC_LIST_TYPE item);

/* returns an item from the list by index */
sc_inline _SC_LIST_METHOD(_SC_LIST_TYPE, get)(_SC_LIST_TYPENAME *list, size_t idx)
{
    assert(idx < list->size);
    return list->items[idx];
}

#include "_sc_list_post.inc"
