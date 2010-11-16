#include "sc_test.h"
#include "sc_list.h"

sc_test(basic_interface)
{
    sc_intlist_t *list = sc_intlist_new(1);
    sc_intlist_append(list, 10);
    sc_intlist_append(list, 20);
    sc_intlist_append(list, 30);

    sc_assert_equal(list->items[0], 10);
    sc_assert_equal(list->items[1], 20);
    sc_assert_equal(list->items[2], 30);
    sc_assert_equal(list->size, 3);
    sc_assert(list->allocated >= 3);
}

sc_testsetup()
{
    sc_testgroup(list) {
        sc_run_test(basic_interface);
    }
}
