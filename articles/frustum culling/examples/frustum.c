/*    Copyright (C) 2005 Robert Kooima                                       */
/*                                                                           */
/*    ELECTRO is free software;  you can redistribute it and/or modify it    */
/*    under the terms of the  GNU General Public License  as published by    */
/*    the  Free Software Foundation;  either version 2 of the License, or    */
/*    (at your option) any later version.                                    */
/*                                                                           */
/*    This program is distributed in the hope that it will be useful, but    */
/*    WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of    */
/*    MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU    */
/*    General Public License for more details.                               */

#include <math.h>

#include "opengl.h"
#include "matrix.h"
#include "frustum.h"

/*---------------------------------------------------------------------------*/

static int tst_plane(const float V[4], const float b[6])
{
    const float k00 = b[0] * V[0];
    const float k11 = b[1] * V[1];
    const float k22 = b[2] * V[2];
    const float k30 = b[3] * V[0];
    const float k41 = b[4] * V[1];
    const float k52 = b[5] * V[2];

    int c = 0;

    /* Test all 8 points of the bounding box against this plane. */

    if (k00 + k11 + k22 + V[3] > 0) c++;
    if (k00 + k11 + k52 + V[3] > 0) c++;
    if (k00 + k41 + k22 + V[3] > 0) c++;
    if (k00 + k41 + k52 + V[3] > 0) c++;
    if (k30 + k11 + k22 + V[3] > 0) c++;
    if (k30 + k11 + k52 + V[3] > 0) c++;
    if (k30 + k41 + k22 + V[3] > 0) c++;
    if (k30 + k41 + k52 + V[3] > 0) c++;

    /* Return the number of points in front of the plane. */

    return c;
}

int test_frustum(float V[6][4], const float b[6])
{
    int c0, c1, c2, c3, c4, c5;

    /* If the bounding box is entirely behind any of the planes, return -1. */

    if ((c0 = tst_plane(V[0], b)) == 0) return -1;
    if ((c1 = tst_plane(V[1], b)) == 0) return -1;
    if ((c2 = tst_plane(V[2], b)) == 0) return -1;
    if ((c3 = tst_plane(V[3], b)) == 0) return -1;
    if ((c4 = tst_plane(V[4], b)) == 0) return -1;
    if ((c5 = tst_plane(V[5], b)) == 0) return -1;

    /* If the box is entirely in view, return +1.  If split, return 0. */

    return (c0 + c1 + c2 + c3 + c4 + c5 == 48) ? 1 : 0;
}

void get_viewfrust(float V[6][4])
{
    float P[16], M[16], X[16];
    int i;

    /* Acquire the model-view-projection matrix. */

    glGetFloatv(GL_PROJECTION_MATRIX, P);
    glGetFloatv(GL_MODELVIEW_MATRIX,  M);

    mult_mat_mat(X, P, M);

    /* Left plane. */

    V[0][0] = X[3]  + X[0];
    V[0][1] = X[7]  + X[4];
    V[0][2] = X[11] + X[8];
    V[0][3] = X[15] + X[12];

    /* Right plane. */

    V[1][0] = X[3]  - X[0];
    V[1][1] = X[7]  - X[4];
    V[1][2] = X[11] - X[8];
    V[1][3] = X[15] - X[12];

    /* Bottom plane. */

    V[2][0] = X[3]  + X[1];
    V[2][1] = X[7]  + X[5];
    V[2][2] = X[11] + X[9];
    V[2][3] = X[15] + X[13];

    /* Top plane. */

    V[3][0] = X[3]  - X[1];
    V[3][1] = X[7]  - X[5];
    V[3][2] = X[11] - X[9];
    V[3][3] = X[15] - X[13];

    /* Near plane. */

    V[4][0] = X[3]  + X[2];
    V[4][1] = X[7]  + X[6];
    V[4][2] = X[11] + X[10];
    V[4][3] = X[15] + X[14];

    /* Far plane. */

    V[5][0] = X[3]  - X[2];
    V[5][1] = X[7]  - X[6];
    V[5][2] = X[11] - X[10];
    V[5][3] = X[15] - X[14];

    /* Normalize all plane vectors. */

    for (i = 0; i < 6; ++i)
    {
        float k = (float) sqrt(DOT3(V[i], V[i]));

        V[i][0] /= k;
        V[i][1] /= k;
        V[i][2] /= k;
        V[i][3] /= k;
    }
}

void get_viewpoint(float p[4])
{
    float M[16], I[16];

    /* View position. */

    glGetFloatv(GL_MODELVIEW_MATRIX, M);

    load_inv(I, M);

    p[0] =  I[12];
    p[1] =  I[13];
    p[2] =  I[14];
    p[3] =  I[15];
}
