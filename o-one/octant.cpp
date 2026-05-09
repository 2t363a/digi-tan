#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <random>
#include <stdexcept>

#include "inlines.hpp"

// #define XX 1

static inline
int get_octant(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;

# if XX
    fprintf(stdout, "%d %d\n", dx, dy);
# endif

    if (dx > 0 and dy >= 0) { return (dx <= dy) ? 1 : 0; }
    else if (dx <= 0 and dy > 0) { return (-dx < dy) ? 2 : 3; }
    else if (dx < 0 and dy <= 0) { return (-dx <= -dy) ? 5 : 4; }
    else if (dx >= 0 and dy < 0) { return (dx < -dy) ? 6 : 7; }

    return 0;
}

static
int get_octant2(int x1, int y1, int x2, int y2) {
    static constexpr long double k4OverPi = 0xa.2f9836e4e44152ap-3L;
    static constexpr long double k4OverPi_2 = -0xf.62a0b82b2c88fc9p-73L;
    static constexpr long double k2Pi = 0xc.90fdaa22168c235p-1L;
    static constexpr long double k2Pi_2 = -0xe.ce675d1fc8f8cbbp-67L;

    long double dx = x2 - x1;
    long double dy = y2 - y1;
    long double phi = ::atan2l(dy, dx);
    long double oct = std::fma(k4OverPi, phi, std::fma(phi, k4OverPi_2, 0.0L));
    if (oct < 0.0) { oct += 8.0; }
    int i_oct = ::truncl(oct);
# if XX
    printf("oct = %32.20Lf, i_oct = %d, deg = %32.20Lf\n", oct, i_oct, phi * 360 / k2Pi);
# endif
    return i_oct;
}

static
int search() {
    struct X { int x1; int y1; int x2; int y2; int q; };
    int width = 10'000;
    int height = 10'000;

    uint64_t seed = x_rdrand64();
    fprintf(stderr, "; seed = %016lX\n", seed);
    std::mt19937_64 rng(seed);

    long n0 = 0;
    long n = 0;
    long n_err = 0;

    std::array<X, 100> arr;
    std::fill(arr.begin(), arr.end(), X { .x1 = 0, .y1 = 0, .x2 = 0, .y2 = 0, .q = -1 });
    while (1) {
        double d_x1 = x_fma3(0x1p-53, (rng() >> 11), 0.0);
        double d_y1 = x_fma3(0x1p-53, (rng() >> 11), 0.0);
        double d_x2 = x_fma3(0x1p-53, (rng() >> 11), 0.0);
        double d_y2 = x_fma3(0x1p-53, (rng() >> 11), 0.0);

        int x1 = std::trunc(d_x1 * width);
        int y1 = std::trunc(d_y1 * height);
        int x2 = std::trunc(d_x2 * width);
        int y2 = std::trunc(d_y2 * height);

        n0 += 1;
        /// special values
        auto v = (rng() >> 60);
        switch (v) {
            case 0: x1 = x2; y1 = y2; break;
            case 1: x1 = x2; break;
            case 2: y1 = y2; break;
            case 3: x2 = width - 1 - x1; y2 = height - 1 - y1; break;
            default:
                n += 1;
                break;
        }

        int o1 = get_octant(x1, y1, x2, y2);
        int o2 = get_octant2(x1, y1, x2, y2);
        if (arr[o1].q == -1) {
                arr[o1] = { .x1 = x1, .y1 = y1, .x2 = x2, .y2 = y2, .q = o2 };
        } else {
            if (arr[o1].q != o2) {
                fprintf(stdout, "initial, %d, *%d, %d, %d, %d, %d\n", o1, arr[o1].q, arr[o1].x1, arr[o1].y1, arr[o1].x2, arr[o1].y2);
                fprintf(stdout, "following, %d, *%d, %d, %d, %d, %d\n", o1, o2, x1, y1, x2, y2);
                break;
            }
        }
        if (o1 != o2)
            fprintf(stdout, "%d, %d, %d, %d, %d, %d\n", o1, o2, x1, y1, x2, y2);
        fprintf(stderr, "%12ld:%12ld\r", n,  n0);
    }

    return 0;
}

int main(int argc, const char* argv[]) try {
    if (argc != 5) { return search(); }
    int a[4] = {0};
    for (int i = 1; i < 5; ++i) { a[i - 1] = std::stoi(argv[i]); }

    int o1 = get_octant(a[0], a[1], a[2], a[3]);
    int o2 = get_octant2(a[0], a[1], a[2], a[3]);

    fprintf(stderr, "octant, %d, octant2, %d, x1, %d, y1, %d, x2, %d, y2, %d\n", o1, o2, a[0], a[1], a[2], a[3]);
    return 0;
} catch (std::invalid_argument const& e) {
    fprintf(stderr, "integer expected, %s\n", e.what());
    std::exit(127);
}


