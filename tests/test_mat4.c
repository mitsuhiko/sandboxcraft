#include "sc_test.h"
#include "sc_mat4.h"

sc_test(basic_interface)
{
    sc_mat4_t mat;

    sc_mat4_set(&mat,
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 1.5f, 2.5f, 3.5f,
        4.5f, 5.5f, 6.5f, 7.5f
    );

    sc_assert_almost_equal(mat.m[0][0], 1.0f);
    sc_assert_almost_equal(mat.m[0][1], 2.0f);
    sc_assert_almost_equal(mat.m[0][2], 3.0f);
    sc_assert_almost_equal(mat.m[0][3], 4.0f);
    sc_assert_almost_equal(mat.m[1][0], 5.0f);
    sc_assert_almost_equal(mat.m[1][1], 6.0f);
    sc_assert_almost_equal(mat.m[1][2], 7.0f);
    sc_assert_almost_equal(mat.m[1][3], 8.0f);
    sc_assert_almost_equal(mat.m[2][0], 9.0f);
    sc_assert_almost_equal(mat.m[2][1], 1.5f);
    sc_assert_almost_equal(mat.m[2][2], 2.5f);
    sc_assert_almost_equal(mat.m[2][3], 3.5f);
    sc_assert_almost_equal(mat.m[3][0], 4.5f);
    sc_assert_almost_equal(mat.m[3][1], 5.5f);
    sc_assert_almost_equal(mat.m[3][2], 6.5f);
    sc_assert_almost_equal(mat.m[3][3], 7.5f);

    sc_assert_almost_equal(mat.elms[0], 1.0f);
    sc_assert_almost_equal(mat.elms[1], 2.0f);
    sc_assert_almost_equal(mat.elms[2], 3.0f);
    sc_assert_almost_equal(mat.elms[3], 4.0f);
    sc_assert_almost_equal(mat.elms[4], 5.0f);
    sc_assert_almost_equal(mat.elms[5], 6.0f);
    sc_assert_almost_equal(mat.elms[6], 7.0f);
    sc_assert_almost_equal(mat.elms[7], 8.0f);
    sc_assert_almost_equal(mat.elms[8], 9.0f);
    sc_assert_almost_equal(mat.elms[9], 1.5f);
    sc_assert_almost_equal(mat.elms[10], 2.5f);
    sc_assert_almost_equal(mat.elms[11], 3.5f);
    sc_assert_almost_equal(mat.elms[12], 4.5f);
    sc_assert_almost_equal(mat.elms[13], 5.5f);
    sc_assert_almost_equal(mat.elms[14], 6.5f);
    sc_assert_almost_equal(mat.elms[15], 7.5f);

    sc_mat4_set_identity(&mat);
    sc_assert_almost_equal(mat.m[0][0], 1.0f);
    sc_assert_almost_equal(mat.m[0][1], 0.0f);
    sc_assert_almost_equal(mat.m[0][2], 0.0f);
    sc_assert_almost_equal(mat.m[0][3], 0.0f);
    sc_assert_almost_equal(mat.m[1][0], 0.0f);
    sc_assert_almost_equal(mat.m[1][1], 1.0f);
    sc_assert_almost_equal(mat.m[1][2], 0.0f);
    sc_assert_almost_equal(mat.m[1][3], 0.0f);
    sc_assert_almost_equal(mat.m[2][0], 0.0f);
    sc_assert_almost_equal(mat.m[2][1], 0.0f);
    sc_assert_almost_equal(mat.m[2][2], 1.0f);
    sc_assert_almost_equal(mat.m[2][3], 0.0f);
    sc_assert_almost_equal(mat.m[3][0], 0.0f);
    sc_assert_almost_equal(mat.m[3][1], 0.0f);
    sc_assert_almost_equal(mat.m[3][2], 0.0f);
    sc_assert_almost_equal(mat.m[3][3], 1.0f);
}

sc_test(inverse)
{
    sc_mat4_t mat, expected;
    sc_mat4_set(&mat,
        2.0f, 1.0f, 4.0f, 3.0f,
        1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    );
    sc_mat4_set(&expected,
         1.0f,  3.0f, -1.0f, -3.0f,
        -1.0f, -2.0f,  1.0f,  3.0f,
         0.0f, -1.0f,  1.0f,  0.0f,
         0.0f,  0.0f, -1.0f,  1.0f
    );

    sc_mat4_inverse(&mat, &mat);
    sc_assert(sc_mat4_equal(&mat, &expected));
    mat.elms[0] = 25.0f;
    sc_assert(!sc_mat4_equal(&mat, &expected));
}

sc_test(multiplication)
{
    sc_mat4_t mat1, mat2, expected;
    sc_mat4_set(&mat1,
        2.0f, 1.0f, 4.0f, 3.0f,
        1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    );
    sc_mat4_set(&mat2,
        2.0f, 1.0f, 4.0f, 3.0f,
        2.0f, 1.0f, 0.0f, 0.0f,
        3.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 4.0f
    );
    sc_mat4_set(&expected,
        21.0f, 10.0f, 15.0f, 18.0f,
         4.0f,  2.0f,  4.0f,  3.0f,
         7.0f,  3.0f,  5.0f,  3.0f,
         8.0f,  4.0f,  6.0f,  7.0f
    );
    sc_mat4_mul(&mat1, &mat1, &mat2);
    sc_assert(sc_mat4_equal(&mat1, &expected));
}

sc_test(transpose)
{
    sc_mat4_t mat;
    sc_mat4_set(&mat,
        1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f
    );
    sc_mat4_transpose(&mat, &mat);
}

sc_testsetup()
{
    sc_testgroup(mat4) {
        sc_run_test(basic_interface);
        sc_run_test(inverse);
        sc_run_test(multiplication);
        sc_run_test(transpose);
    }
}
