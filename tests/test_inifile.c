#include "sc_test.h"
#include "sc_inifile.h"
#include "sc_path.h"

#define TESTFILE "__sc_test.ini"

static void
common_assertions(sc_inifile_t *ini)
{
    sc_assert(strcmp(sc_inifile_get_string(ini, "general.foo", ""),
                     "a value") == 0);
    sc_assert(strcmp(sc_inifile_get_string(ini, "general.bar", ""),
                     "another value") == 0);
    sc_assert(strcmp(sc_inifile_get_string(ini, "other.intfoo", ""),
                     "1") == 0);
    sc_assert(strcmp(sc_inifile_get_string(ini, "other.boolfoo", ""),
                     "true") == 0);

    sc_assert_equal(sc_inifile_get_int(ini, "general.foo", 0), 0);
    sc_assert_equal(sc_inifile_get_int(ini, "other.intfoo", 42), 1);
    sc_assert_equal(sc_inifile_get_float(ini, "other.floatfoo", 42.0f), 1.0f);
}

static void
delete_testfile(void)
{
    sc_path_delete_file(TESTFILE);
}

sc_test(basic_interface)
{
    sc_inifile_t *ini = sc_new_inifile();

    sc_inifile_set_string(ini, "general.foo", "a value");
    sc_inifile_set_string(ini, "general.bar", "another value");
    sc_inifile_set_int(ini, "other.intfoo", 1);
    sc_inifile_set_bool(ini, "other.boolfoo", 42);
    sc_inifile_set_float(ini, "other.floatfoo", 1.0f);
    common_assertions(ini);

    sc_assert(sc_inifile_save(ini, TESTFILE));
    sc_free_inifile(ini);

    sc_assert(!sc_inifile_from_filename(TESTFILE "bullshit"));
    sc_assert(sc_get_errno() == SC_ENOENT);

    ini = sc_inifile_from_filename(TESTFILE);
    common_assertions(ini);
    sc_free_inifile(ini);

    atexit(delete_testfile);
}

sc_test(keycheck)
{
    sc_assert(sc_inifile_key_is_valid("foo.bar"));
    sc_assert(!sc_inifile_key_is_valid("foo"));
}

sc_testsetup()
{
    sc_testgroup(inifile) {
        sc_run_test(basic_interface);
        sc_run_test(keycheck);
    }
}
