#include "_sc_list_pre.inc"
#include "sc_utils.h"

_SC_LIST_TYPENAME *_SC_LIST_NEW_METHOD(size_t initial_size)
{
    _SC_LIST_TYPENAME *rv = sc_xalloc(_SC_LIST_TYPENAME);
    if (!initial_size)
        initial_size = 32;
    rv->size = 0;
    rv->allocated = initial_size;
    rv->items = sc_xmalloc(sizeof(_SC_LIST_TYPE) * initial_size);
    return rv;
}

void _SC_LIST_FREE_METHOD(_SC_LIST_TYPENAME *list)
{
    if (!list)
        return;
    sc_free(list->items);
    sc_free(list);
}

void _SC_LIST_METHOD(append)(_SC_LIST_TYPENAME *list, _SC_LIST_TYPE item)
{
    if (list->size >= list->allocated) {
        size_t new_size = (size_t)(list->allocated * 1.33f);
        if (new_size == list->allocated)
            new_size++;
        list->items = sc_xrealloc(list->items, sizeof(_SC_LIST_TYPE) * new_size);
        list->allocated = new_size;
    }
    list->items[list->size++] = item;
}

static int
_SC_LIST_METHOD(default_cmpfunc)(const _SC_LIST_TYPE *v1,
                                 const _SC_LIST_TYPE *v2, void *closure)
{
    const _SC_LIST_TYPE a = *v1;
    const _SC_LIST_TYPE b = *v2;
    return (a == b) ? 0 : (a < b) ? -1 : 1;
}

void _SC_LIST_METHOD(sort)(_SC_LIST_TYPENAME *list,
                           _SC_LIST_METHOD(cmpfunc) cmpfunc,
                           void *closure)
{
    /* if no cmp function is provided we go with the default for this list */
    if (cmpfunc == 0)
        cmpfunc = _SC_LIST_METHOD(default_cmpfunc);
    sc_sort(list->items, list->size, sizeof(_SC_LIST_TYPE), closure,
            (sc_cmpfunc)cmpfunc);
}

#include "_sc_list_post.inc"
