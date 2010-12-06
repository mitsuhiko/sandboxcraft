#include "sc_test.h"
#include "sc_strbuf.h"

sc_test(basic_interface)
{
    int i;
    char *rv;
    char compare[2048];
    size_t length;

    sc_strbuf_t *buf = sc_new_strbuf();
    sc_strbuf_append(buf, "Hello ");
    sc_strbuf_append(buf, "World!");
    
    rv = sc_strbuf_get_contents(buf, NULL);
    sc_assert(strcmp(rv, "Hello World!") == 0);
    sc_free(rv);
    sc_free_strbuf(buf);

    buf = sc_new_strbuf();
    for (i = 0; i < 2000; i++) {
        char c[2] = {(char)(i % 255) + 1, 0};
        compare[i] = *c;
        sc_strbuf_append(buf, c);
    }
    compare[2000] = '\0';
    rv = sc_free_strbuf_and_get_contents(buf, &length);
    sc_assert_equal(length, 2000);
    sc_assert(strcmp(rv, compare) == 0);
}

sc_test(special_operations)
{
    char *rv;
    sc_strbuf_t *buf = sc_new_strbuf();
    sc_strbuf_appendf(buf, "Hello %s?", "World");
    sc_strbuf_appendc(buf, '!');
    rv = sc_free_strbuf_and_get_contents(buf, NULL);
    sc_assert(strcmp(rv, "Hello World?!") == 0);
}

sc_testsetup()
{
    sc_testgroup(strbuf) {
        sc_run_test(basic_interface);
        sc_run_test(special_operations);
    }
}
