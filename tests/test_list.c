#include "sc_test.h"
#include "sc_list.h"

sc_test(basic_interface)
{
    sc_intlist_t *list = sc_new_intlist(1);
    sc_intlist_append(list, 10);
    sc_intlist_append(list, 20);
    sc_intlist_append(list, 30);

    sc_assert_equal(list->items[0], 10);
    sc_assert_equal(sc_intlist_get(list, 0), 10);
    sc_assert_equal(list->items[1], 20);
    sc_assert_equal(sc_intlist_get(list, 1), 20);
    sc_assert_equal(list->items[2], 30);
    sc_assert_equal(sc_intlist_get(list, 2), 30);
    sc_assert_equal(list->size, 3);
    sc_assert(list->allocated >= 3);

    sc_free_intlist(list);
}

sc_test(sorting_integers)
{
    sc_intlist_t *list = sc_new_intlist(1);
    sc_intlist_append(list, 5);
    sc_intlist_append(list, 2);
    sc_intlist_append(list, 1);
    sc_intlist_append(list, 12);
    sc_intlist_append(list, 42);
    sc_intlist_sort(list, NULL, NULL);
    sc_assert_equal(list->items[0], 1);
    sc_assert_equal(list->items[1], 2);
    sc_assert_equal(list->items[2], 5);
    sc_assert_equal(list->items[3], 12);
    sc_assert_equal(list->items[4], 42);
    sc_free_intlist(list);
}

sc_testsetup()
{
    sc_testgroup(list) {
        sc_run_test(basic_interface);
        sc_run_test(sorting_integers);
    }
}
