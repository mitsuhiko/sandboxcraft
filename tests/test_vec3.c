#include "sc_test.h"
#include "sc_vec3.h"

sc_test(basic_manipulation)
{
    sc_vec3_t vec;
    sc_vec3_set(&vec, 1.0f, 2.0f, 3.0f);

    sc_assert_almost_equal(vec.x, 1.0f);
    sc_assert_almost_equal(vec.y, 2.0f);
    sc_assert_almost_equal(vec.z, 3.0f);
}

sc_testsetup()
{
    sc_testgroup(Vector3) {
        sc_run_test(basic_manipulation);
    }
}
