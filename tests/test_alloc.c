#include "sc_test.h"
#include "sc_alloc.h"

sc_test(stackalloc_usage)
{
    sc_stackalloc_t alloc;
    char memory[1024];
    char *foo;
    int *bar;

    sc_stackalloc_init(&alloc, &memory, 1024);

    foo = sc_stackalloc_alloc(&alloc, 128);
    sc_assert_equal((uintptr_t)foo, (uintptr_t)memory);
    bar = sc_stackalloc_alloc(&alloc, sizeof(int));
    sc_assert_equal((uintptr_t)bar, (uintptr_t)memory + 128);
    sc_assert(sc_stackalloc_managed(&alloc, foo));
    sc_assert(!sc_stackalloc_managed(&alloc, "something else"));
}

sc_testsetup()
{
    sc_testgroup(stack_allocator) {
        sc_run_test(stackalloc_usage);
    }
}
