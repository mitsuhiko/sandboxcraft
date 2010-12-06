#include "sc_test.h"
#include "sc_path.h"

sc_test(dirname)
{
    char *dirname;
    
    dirname = sc_path_dirname("/foo/bar/test.txt");
    sc_assert(strcmp(dirname, "/foo/bar") == 0);
    sc_free(dirname);

    dirname = sc_path_dirname("test.txt");
    sc_assert(strcmp(dirname, ".") == 0);
    sc_free(dirname);
}

sc_test(join)
{
    char *path;

    path = sc_path_join("/foo/bar", "baz.txt");
    sc_assert(strcmp(path, "/foo/bar/baz.txt") == 0);
    sc_free(path);

    path = sc_path_join_with_dir("/foo/bar/test.txt", "aha.txt");
    sc_assert(strcmp(path, "/foo/bar/aha.txt") == 0);
    sc_free(path);
}

sc_testsetup()
{
    sc_testgroup(path) {
        sc_run_test(dirname);
        sc_run_test(join);
    }
}
