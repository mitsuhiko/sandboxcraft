#include <unistd.h>
#include <stdarg.h>
#include "sc_test.h"

static struct {
    int group_entered;
    int group_tests;
    int group_failed;
    int test_failed;
    int total_failed;
    int segfault;
} context;

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
    context.test_failed = 0;
    context.group_tests++;
    printf("  - %s\n", name);
    func();
    if (context.test_failed) {
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
    context.test_failed = 1;
    if (context.segfault)
        SC_SEGFAULT;
}

int
main(int argc, char **argv)
{
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

    _sc_run_tests();

    if (context.total_failed)
        printf("FAIL(errors=%d)\n", context.total_failed);
    else
        printf("OK.\n");

    return !!context.total_failed;
}
