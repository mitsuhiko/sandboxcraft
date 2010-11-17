#include "_sc_list_pre.inc"


_SC_LIST_TYPENAME *
_SC_LIST_NEW_METHOD(size_t initial_size)
{
    _SC_LIST_TYPENAME *rv = sc_xalloc(_SC_LIST_TYPENAME);
    if (!initial_size)
        initial_size = 32;
    rv->size = 0;
    rv->allocated = initial_size;
    rv->items = sc_xmalloc(sizeof(_SC_LIST_TYPE) * initial_size);
    return rv;
}

void
_SC_LIST_FREE_METHOD(_SC_LIST_TYPENAME *list)
{
    if (!list)
        return;
    sc_free(list->items);
    sc_free(list);
}

void
_SC_LIST_METHOD(append)(_SC_LIST_TYPENAME *list, _SC_LIST_TYPE item)
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

int
_SC_LIST_METHOD(remove)(_SC_LIST_TYPENAME *list, _SC_LIST_TYPE item)
{
    ssize_t idx = _SC_LIST_METHOD(find)(list, item);
    if (idx < 0)
        return 0;
    _SC_LIST_METHOD(delete)(list, idx);
    return 1;
}

_SC_LIST_TYPE
_SC_LIST_METHOD(pop)(_SC_LIST_TYPENAME *list)
{
    assert(list->size > 0);
    return list->items[--list->size];
}

void
_SC_LIST_METHOD(delete)(_SC_LIST_TYPENAME *list, size_t idx)
{
    assert(idx < list->size);
    memmove(list->items + idx, list->items + idx + 1,
            sizeof(_SC_LIST_TYPE) * (list->size - idx - 1));
    list->size--;
}

void
_SC_LIST_METHOD(trim)(_SC_LIST_TYPENAME *list)
{
    size_t length = (size_t)(list->size * 1.33f);
    if (length > list->allocated)
        return;
    list->allocated = length;
    list->items = sc_xrealloc(list->items, list->allocated);
}

void
_SC_LIST_METHOD(reverse)(_SC_LIST_TYPENAME *list)
{
    size_t i, j;
    if (list->size <= 1)
        return;
    for (i = 0, j = list->size - 1; i < j; i++, j--) {
        _SC_LIST_TYPE tmp = list->items[i];
        list->items[i] = list->items[j];
        list->items[j] = tmp;
    }
}

static int
_SC_LIST_METHOD(default_cmpfunc)(const void *v1, const void *v2,
                                 void *closure)
{
    const _SC_LIST_TYPE a = *(const _SC_LIST_TYPE *)v1;
    const _SC_LIST_TYPE b = *(const _SC_LIST_TYPE *)v2;
    return (a == b) ? 0 : (a < b) ? -1 : 1;
}

void
_SC_LIST_METHOD(sort)(_SC_LIST_TYPENAME *list, sc_cmpfunc cmpfunc,
                      void *closure)
{
    /* if no cmp function is provided we go with the default for this list */
    if (cmpfunc == 0)
        cmpfunc = _SC_LIST_METHOD(default_cmpfunc);
    sc_sort(list->items, list->size, sizeof(_SC_LIST_TYPE), closure, cmpfunc);
}

void
_SC_LIST_METHOD(extend)(_SC_LIST_TYPENAME *list,
                        _SC_LIST_TYPENAME *other_list)
{
    size_t min_size = (list->size + other_list->size);
    if (min_size < list->allocated) {
        list->allocated = min_size;
        list->items = sc_xrealloc(list->items,
                                  list->allocated * sizeof(_SC_LIST_TYPE));
    }
    memcpy(list->items + list->size, other_list->items,
           other_list->size * sizeof(_SC_LIST_TYPE));
    list->size += other_list->size;
}

_SC_LIST_TYPENAME *
_SC_LIST_METHOD(clone)(_SC_LIST_TYPENAME *list)
{
    _SC_LIST_TYPENAME *rv = sc_alloc(_SC_LIST_TYPENAME);
    rv->size = list->size;
    rv->allocated = list->size;
    rv->items = sc_xmalloc(sizeof(_SC_LIST_TYPE) * rv->size);
    memcpy(rv->items, list->items, sizeof(_SC_LIST_TYPE) * rv->size);
    return rv;
}

#include "_sc_list_post.inc"
