#include "sc_test.h"
#include "sc_vec3.h"

sc_test(basic_interface)
{
    const float *vecptr;
    sc_vec3_t vec;
    sc_vec3_set(&vec, 1.0f, 2.0f, 3.0f);

    sc_assert_almost_equal(vec.x, 1.0f);
    sc_assert_almost_equal(vec.y, 2.0f);
    sc_assert_almost_equal(vec.z, 3.0f);

    vecptr = (const float *)&vec.x;
    sc_assert_almost_equal(vecptr[0], 1.0f);
    sc_assert_almost_equal(vecptr[1], 2.0f);
    sc_assert_almost_equal(vecptr[2], 3.0f);

    sc_vec3_zero(&vec);
    sc_assert(sc_vec3_is_zero(&vec));
}

sc_test(basic_math)
{
    sc_vec3_t vec1, vec2;

    sc_vec3_set(&vec1, 1.0f, 2.0f, 3.0f);
    sc_vec3_set(&vec2, 3.0f, 2.0f, 1.0f);
    sc_vec3_sub(&vec1, &vec1, &vec2);

    sc_assert_almost_equal(vec1.x, -2.0f);
    sc_assert_almost_equal(vec1.y, 0.0f);
    sc_assert_almost_equal(vec1.z, 2.0f);

    sc_vec3_add(&vec1, &vec1, &vec1);
    sc_assert_almost_equal(vec1.x, -4.0f);
    sc_assert_almost_equal(vec1.y, 0.0f);
    sc_assert_almost_equal(vec1.z, 4.0f);
}

sc_testsetup()
{
    sc_testgroup(Vector3) {
        sc_run_test(basic_interface);
        sc_run_test(basic_math);
    }
}
