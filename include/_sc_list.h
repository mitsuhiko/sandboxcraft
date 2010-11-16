#include "_sc_list_pre.inc"

typedef struct {
    size_t size;
    size_t allocated;
    _SC_LIST_TYPE *items;
} _SC_LIST_TYPENAME;

/* creates a new list */
_SC_LIST_TYPENAME *_SC_LIST_METHOD(new)(size_t initial_size);

/* frees the list */
void _SC_LIST_METHOD(free)(_SC_LIST_TYPENAME *list);

/* adds a new item to the list */
void _SC_LIST_METHOD(append)(_SC_LIST_TYPENAME *list, _SC_LIST_TYPE item);

#include "_sc_list_post.inc"
