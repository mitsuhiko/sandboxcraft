/* implements simplex perlin noises based on this paper:
   http://staffwww.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf */
#include "sc_perlin.h"
#include "sc_vec3.h"
#include "sc_rnd.h"
#include "sc_math.h"

/* XXX: table is from the paper.  why is 9 twice in here? */
static const unsigned char default_permutation_table[256] = {
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
    140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148,
    247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32,
    57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68,
    175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111,
    229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208,
    89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109,
    198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147,
    118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182,
    189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70,
    221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 9, 98, 108,
    110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251,
    34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235,
    249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204,
    176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114,
    67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};
#define TABLE_SIZE sizeof(default_permutation_table)

static const float gradient_vectors[48] = {
    1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 
    1.0f, 0.0f, 1.0f,
    -1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, -1.0f,
    -1.0f, 0.0f, -1.0f, 
    0.0f, 1.0f, 1.0f,
    0.0f, -1.0f, 1.0f,
    0.0f, 1.0f, -1.0f,
    0.0f, -1.0f, -1.0f, 
    1.0f, 1.0f, 0.0f,
    0.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 0.0f,
    0.0f, -1.0f, -1.0f
};

#define PT(I) perlin->permutation_table[(I) % perlin->period]
#define GRAD(I) ((sc_vec3_t *)((float *)gradient_vectors + ((I) * 3)))
#define F2 (0.5f * (sqrtf(3.0f) - 1.0f))
#define G2 ((3.0f - sqrtf(3.0f)) / 6.0f)
#define F3 (1.0f / 3.0f)
#define G3 (1.0f / 6.0f)

static void
randomize_table(unsigned char *table, size_t size, uint32_t seed)
{
    size_t i, j;
    unsigned char tmp;
    sc_fast_rnd_t rnd;
    sc_fast_rnd_seed(&rnd, seed);

    for (i = 0; i < size; i++) {
        j = sc_fast_rnd_next_index(&rnd, size);
        tmp = table[i];
        table[i] = table[j];
        table[j] = tmp;
    }
}

sc_perlin_t *
sc_new_perlin(uint32_t seed)
{
    sc_perlin_t *rv = sc_xalloc(sc_perlin_t);
    rv->permutation_table = sc_xmalloc(TABLE_SIZE);
    rv->period = TABLE_SIZE;
    memcpy(rv->permutation_table, default_permutation_table, TABLE_SIZE);
    randomize_table(rv->permutation_table, TABLE_SIZE, seed);
    return rv;
}

void
sc_free_perlin(sc_perlin_t *perlin)
{
    if (!perlin)
        return;
    sc_free(perlin->permutation_table);
    sc_free(perlin);
}

float
sc_perlin_noise2(const sc_perlin_t *perlin, float x, float y)
{
    int i1, j1, ii, jj, gi0, gi1, gi2;
    float x1, y1, x2, y2, tt, noise = 0.0f;
    float s = (x + y) * F2;
    int i = (int)floorf(x + s);
    int j = (int)floorf(y + s);
    float t = (i + j) * G2;
    float x0 = x - (i - t);
    float y0 = y - (j - t);

#define TRIANGLE(I1, J1) do { \
    i1 = I1; j1 = J1; \
} while (0)
#define ADMIX2(X, Y, G) do { \
    tt = 0.5f - powf(X, 2.0f) - powf(Y, 2.0f); \
    if (tt > 0.0f) { \
        const sc_vec3_t *g = GRAD(G); \
        noise += powf(tt, 4.0f) * (g->x * X + g->y * Y); \
    } \
} while (0)

    if (x0 > y0)
        TRIANGLE(1, 0);
    else
        TRIANGLE(0, 1);

    x1 = x0 - i1 + G2;
    y1 = y0 - j1 + G2;
    x2 = x0 + G2 * 2.0f - 1.0f;
    y2 = y0 + G2 * 2.0f - 1.0f;
    ii = (int)i % perlin->period;
    jj = (int)j % perlin->period;
    gi0 = PT(ii + PT(jj)) % 12;
    gi1 = PT(ii + i1 + PT(jj + j1)) % 12;
    gi2 = PT(ii + 1 + PT(jj + 1)) % 12;

    ADMIX2(x0, y0, gi0);
    ADMIX2(x1, y1, gi1);
    ADMIX2(x2, y2, gi2);

    return noise * 70.0f;
}

float
sc_perlin_noise3(const sc_perlin_t *perlin, float x, float y, float z)
{
    int i1, j1, k1, i2, j2, k2, ii, jj, kk, gi0, gi1, gi2, gi3;
    float x1, y1, z1, x2, y2, z2, x3, y3, z3, tt, noise = 0.0f;
    float s = (x + y + z) * F3;
    int i = (int)floorf(x + s);
    int j = (int)floorf(y + s);
    int k = (int)floorf(z + s);
    float t = (i + j + k) * G3;
    float x0 = x - (i - t);
    float y0 = y - (j - t);
    float z0 = z - (k - t);

#define TETRAHEDRON(I1, J1, K1, I2, J2, K2) do { \
    i1 = I1; j1 = J1; k1 = K1; i2 = I2; j2 = J2; k2 = K2; \
} while (0)
#define ADMIX3(X, Y, Z, G) do { \
    tt = 0.6f - powf(X, 2.0f) - powf(Y, 2.0f) - powf(Z, 2.0f); \
    if (tt > 0.0f) { \
        const sc_vec3_t *g = GRAD(G); \
        noise += powf(tt, 4.0f) * (g->x * X + g->y * Y + g->z * Z); \
    } \
} while (0)

    if (x0 >= y0) {
        if (y0 >= z0)
            TETRAHEDRON(1, 0, 0, 1, 1, 0);
        else if (x0 >= z0)
            TETRAHEDRON(1, 0, 0, 1, 0, 1);
        else
            TETRAHEDRON(0, 0, 1, 1, 0, 1);
    } else {
        if (y0 < z0)
            TETRAHEDRON(0, 0, 1, 0, 1, 1);
        else if (x0 < z0)
            TETRAHEDRON(0, 1, 0, 0, 1, 1);
        else
            TETRAHEDRON(0, 1, 0, 1, 1, 0);
    }

    x1 = x0 - i1 + G3;
    y1 = y0 - j1 + G3;
    z1 = z0 - k1 + G3;
    x2 = x0 - i2 + 2.0f * G3;
    y2 = y0 - j2 + 2.0f * G3;
    z2 = z0 - k2 + 2.0f * G3;
    x3 = x0 - 1.0f + 3.0f * G3;
    y3 = y0 - 1.0f + 3.0f * G3;
    z3 = z0 - 1.0f + 3.0f * G3;
    ii = (int)i % perlin->period;
    jj = (int)j % perlin->period;
    kk = (int)k % perlin->period;
    gi0 = PT(ii + PT(jj + PT(kk))) % 12;
    gi1 = PT(ii + i1 + PT(jj + j1 + PT(kk + k1))) % 12;
    gi2 = PT(ii + i2 + PT(jj + j2 + PT(kk + k2))) % 12;
    gi3 = PT(ii + 1 + PT(jj + 1 + PT(kk + 1))) % 12;

    ADMIX3(x0, y0, z0, gi0);
    ADMIX3(x1, y1, z1, gi1);
    ADMIX3(x2, y2, z2, gi2);
    ADMIX3(x3, y3, z3, gi3);

    return noise * 32.0f;
}

float
sc_perlin_noise2_ex(const sc_perlin_t *perlin, float x, float y,
                    int octaves, float persistence,
                    float base_frequency, float base_amp,
                    float lacunarity)
{
    int i;
    float freq = base_frequency;
    float amp = base_amp;
    float max = 1.0f;
    float total = 1.0f;

    for (i = 0; i < octaves; i++) {
        total += sc_perlin_noise2(perlin, x * freq, y * freq) * amp;
        max += amp;
        freq *= lacunarity;
        amp *= persistence;
    }
    return total / max;
}

float
sc_perlin_noise3_ex(const sc_perlin_t *perlin, float x, float y,
                    float z, int octaves, float persistence,
                    float base_frequency, float base_amp,
                    float lacunarity)
{
    int i;
    float freq = base_frequency;
    float amp = base_amp;
    float max = 1.0f;
    float total = 1.0f;

    for (i = 0; i < octaves; i++) {
        total += sc_perlin_noise3(perlin, x * freq, y * freq, z * freq) * amp;
        max += amp;
        freq *= lacunarity;
        amp *= persistence;
    }
    return total / max;
}
