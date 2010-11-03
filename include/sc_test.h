/* Very basic unittest support */
#ifndef _INC_SC_TEST_H_
#define _INC_SC_TEST_H_

#include "sc_boot.h"

/* defines a new tests and runs them */
#define sc_test(Name) static void _sc_test_##Name()
#define SC_TEST_NAME_OFFSET 9
#define sc_testsetup() void _sc_run_tests()
#define sc_testgroup(Name) \
    if (!sc_test_begin_group(#Name)); else \
    for (; sc_test_end_group(#Name); )
#define sc_run_test(Name) \
    sc_run_actual_test(#Name, &_sc_test_##Name)

/* the context information for a test call */
#define SC_TEST_CONTEXT_INFO \
    __LINE__, __FILE__, (__FUNCTION__ + SC_TEST_NAME_OFFSET)

/* assertions */
#define sc_assert_almost_equal(A, B) do { \
    if (!((A) < ((B) + 1e-6) && (A) > ((B) - 1e-6))) { \
        sc_test_fail(SC_TEST_CONTEXT_INFO, "%s is not almost equal to %s", \
                     #A, #B); \
        return; \
    } \
} while(0)

void sc_test_fail(const int lineno, const char *file, const char *func,
                  const char *format, ...);

int sc_test_begin_group(const char *name);
int sc_test_end_group(const char *name);
void sc_run_actual_test(const char *name, void (*func)());
void _sc_run_tests();

int main(int argc, char **argv);

#endif
