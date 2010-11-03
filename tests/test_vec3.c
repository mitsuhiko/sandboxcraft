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

sc_test(math)
{
    float length, val;
    sc_vec3_t vec1, vec2, vec3;

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

    length = sc_vec3_length(&vec1);
    sc_assert_almost_equal(length, 5.656854f);

    sc_vec3_mul(&vec1, &vec1, 2);
    length = sc_vec3_length(&vec1);
    sc_assert_almost_equal(length, 5.656854f * 2);

    sc_vec3_neg(&vec2);
    sc_assert_almost_equal(vec2.x, -3.0f);
    sc_assert_almost_equal(vec2.y, -2.0f);
    sc_assert_almost_equal(vec2.z, -1.0f);

    sc_vec3_set(&vec3, 2.0f, 4.0f, 3.0f);
    val = sc_vec3_dot(&vec2, &vec3);
    sc_assert_almost_equal(val, -17.0f);
}

sc_test(comparisions)
{
    sc_vec3_t vec1, vec2;

    sc_vec3_set(&vec1, 1.0f, 1.0f, 1.0f);
    sc_vec3_set(&vec2, 1.0f, 1.0f, 1.0f);

    sc_assert(sc_vec3_equal(&vec1, &vec2));
    vec2.z = 2.0f;
    sc_assert(!sc_vec3_equal(&vec1, &vec2));

    sc_assert(sc_vec3_nonzero(&vec1));
    sc_vec3_zero(&vec1);
    sc_assert(!sc_vec3_nonzero(&vec1));
}

sc_testsetup()
{
    sc_testgroup(vec3) {
        sc_run_test(basic_interface);
        sc_run_test(math);
        sc_run_test(comparisions);
    }
}
