#include "_sc_list_pre.inc"

typedef struct {
    size_t size;
    size_t allocated;
    _SC_LIST_TYPE *items;
} _SC_LIST_TYPENAME;

typedef int (*_SC_LIST_METHOD(cmpfunc))(const _SC_LIST_TYPE *v1,
                                        const _SC_LIST_TYPE *v2,
                                        void *closure);

/* creates a new list */
_SC_LIST_TYPENAME *_SC_LIST_NEW_METHOD(size_t initial_size);

/* frees the list */
void _SC_LIST_FREE_METHOD(_SC_LIST_TYPENAME *list);

/* adds a new item to the list */
void _SC_LIST_METHOD(append)(_SC_LIST_TYPENAME *list, _SC_LIST_TYPE item);

/* gets an item from the list */
static sc_inline _SC_LIST_TYPE _SC_LIST_METHOD(get)(_SC_LIST_TYPENAME *list, size_t idx)
{
    assert(idx < list->size);
    return list->items[idx];
}

/* sort a list */
void _SC_LIST_METHOD(sort)(_SC_LIST_TYPENAME *list,
                           _SC_LIST_METHOD(cmpfunc) cmpfunc,
                           void *closure);

#include "_sc_list_post.inc"
