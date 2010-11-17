#include "sc_test.h"
#include "sc_list.h"
#include "sc_rnd.h"

struct test_tuple {
    int index;
};

static int my_sort_func(const void **v1, const void **v2,
                        void *closure)
{
    const struct test_tuple *a = *(const struct test_tuple **)v1;
    const struct test_tuple *b = *(const struct test_tuple **)v2;
    return a->index - b->index;
}

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

sc_test(sorting_small_integer_lists)
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

sc_test(sorting_long_integer_lists)
{
    int i, last_val = 0;
    sc_intlist_t *list = sc_new_intlist(20);
    for (i = 0; i < 1000; i++)
        sc_intlist_append(list, sc_rnd_next_index(20000));
    sc_intlist_sort(list, 0, 0);
    for (i = 0; i < 1000; i++) {
        sc_assert(list->items[i] >= last_val);
        last_val = list->items[i];
    }
    sc_free_intlist(list);
}

sc_test(sorting_with_custom_compare_func)
{
    int i, last_val = 0;
    struct test_tuple *tup;
    sc_list_t *list = sc_new_list(0);

    for (i = 0; i < 1000; i++) {
        tup = sc_xalloc(struct test_tuple);
        tup->index = sc_rnd_next_index(20000);
        sc_list_append(list, tup);
    }
    sc_list_sort(list, my_sort_func, NULL);

    for (i = 0; i < 1000; i++) {
        tup = list->items[i];
        sc_assert(tup->index >= last_val);
        last_val = tup->index;
        sc_free(tup);
    }

    sc_free_list(list);
}

sc_testsetup()
{
    sc_testgroup(list) {
        sc_run_test(basic_interface);
        sc_run_test(sorting_small_integer_lists);
        sc_run_test(sorting_long_integer_lists);
        sc_run_test(sorting_with_custom_compare_func);
    }
}
