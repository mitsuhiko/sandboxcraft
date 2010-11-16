#include "_sc_list_pre.inc"

_SC_LIST_TYPENAME *_SC_LIST_METHOD(new)(size_t initial_size)
{
    _SC_LIST_TYPENAME *rv = sc_xalloc(_SC_LIST_TYPENAME);
    if (!initial_size)
        initial_size = 32;
    rv->size = 0;
    rv->allocated = initial_size;
    rv->items = sc_xmalloc(sizeof(_SC_LIST_TYPE) * initial_size);
    return rv;
}

void _SC_LIST_METHOD(free)(_SC_LIST_TYPENAME *list)
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

#include "_sc_list_post.inc"
