#include "sc_boot.h"
#if SC_PLATFORM != SC_PLATFORM_WINDOWS
#include <unistd.h>
#endif
#include <stdarg.h>
#include <setjmp.h>
#include "sc_test.h"

static struct {
    int group_entered;
    int group_tests;
    int group_failed;
    int total_failed;
    int segfault;
} context;

static jmp_buf current_test_jump;

int
sc_test_begin_group(const char *name)
{
    printf("[%s]\n", name);
    context.group_tests = 0;
    context.group_failed = 0;
    context.group_entered = 0;
    return 1;
}

int
sc_test_end_group(const char *name)
{
    if (!context.group_entered++)
        return 1;
    printf("  > %d test%s, %d failed\n",
           context.group_tests, context.group_tests == 1 ? "" : "s",
           context.group_failed);
    return 0;
}

void
sc_run_actual_test(const char *name, void (*func)())
{
    int rv = setjmp(current_test_jump);
    /* this evaluates to 1 in case the test failed as indicated by the
       sc_test_fail function.  In that case we don't want to call the
       function again but just display the result. */
    if (rv == 0) {
        context.group_tests++;
        printf("  - %s\n", name);
        func();
    } else {
        context.group_failed++;
        context.total_failed++;
    }
}

void
sc_test_fail(const int lineno, const char *file, const char *func,
             const char *format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "    FAIL:\n");
    fprintf(stderr, "      Assertion failed: ");
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n\n      Location: %s:%d\n", file, lineno);
    if (context.segfault)
        SC_SEGFAULT;
    else
        longjmp(current_test_jump, 1);
}

int
main(int argc, char **argv)
{
#if SC_PLATFORM != SC_PLATFORM_WINDOWS
    int c;

    opterr = 0;
    while ((c = getopt(argc, argv, "s")) >= 0)
        switch (c) {
            case 's':
                context.segfault = 1;
                break;
            default:
                fprintf(stderr, "usage: ./test [-s]\n");
                exit(1);
        }
#endif

    _sc_run_tests();

    return !!context.total_failed;
}
