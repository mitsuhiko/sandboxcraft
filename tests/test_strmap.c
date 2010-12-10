#include "sc_test.h"
#include "sc_strmap.h"

sc_test(basic_interface)
{
    sc_strmap_t *strmap = sc_new_strmap();
    sc_assert_equal(sc_strmap_size(strmap), 0);
    sc_strmap_set(strmap, "Foo", (void *)42);
    sc_assert_equal(sc_strmap_get(strmap, "Foo"), (void *)42);
    sc_assert_equal(sc_strmap_get(strmap, "bar"), (void *)0);
    sc_strmap_set(strmap, "Bar", (void *)23);
    sc_assert_equal(sc_strmap_get(strmap, "Foo"), (void *)42);
    sc_assert_equal(sc_strmap_get(strmap, "Bar"), (void *)23);
    sc_strmap_set(strmap, "aha", (void *)13);
    sc_assert_equal(sc_strmap_get(strmap, "Foo"), (void *)42);
    sc_assert_equal(sc_strmap_get(strmap, "Bar"), (void *)23);
    sc_assert_equal(sc_strmap_get(strmap, "aha"), (void *)13);
    sc_assert_equal(sc_strmap_get(strmap, "missing"), (void *)0);
    sc_assert_equal(sc_strmap_size(strmap), 3);
    sc_assert(!sc_strmap_remove(strmap, "missing"));
    sc_assert(sc_strmap_remove(strmap, "aha"));
    sc_assert_equal(sc_strmap_get(strmap, "aha"), (void *)0);
    sc_assert_equal(sc_strmap_size(strmap), 2);
    sc_free_strmap(strmap);
}

static void
iteration_func(const char *key, void *value, void *closure)
{
    *(int *)closure += 1;
    assert((uintptr_t)(key[0] - 'a') == (uintptr_t)value);
}

sc_test(iteration)
{
    int i = 0;

    sc_strmap_t *strmap = sc_new_strmap();
    sc_strmap_set(strmap, "a", (void *)0);
    sc_strmap_set(strmap, "b", (void *)1);
    sc_strmap_set(strmap, "c", (void *)2);
    sc_strmap_set(strmap, "d", (void *)3);

    sc_strmap_each(strmap, iteration_func, &i);
    sc_assert_equal(i, 4);
}

sc_testsetup()
{
    sc_testgroup(strmap) {
        sc_run_test(basic_interface);
        sc_run_test(iteration);
    }
}
