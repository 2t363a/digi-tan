#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <vector>

#include "inlines.hpp"

#if defined(UNROLL)
#define FOR _Pragma("unroll") for
#else
#define FOR for
#endif

static inline
void load(int m) {
    switch (m) {
        case 0: break;
        case 1:
            FOR (int j = 0; j < 10240; ++j)
                asm volatile ("pause;\n" :::);
            break;
        case 2:
            FOR (int j = 0; j < 20480; ++j)
                asm volatile ("pause;\n" :::);
            break;
        case 3:
            FOR (int j = 0; j < 30720; ++j)
                asm volatile ("pause;\n" :::);
            break;
        case 4:
            FOR (int j = 0; j < 40960; ++j)
                asm volatile ("pause;\n" :::);
            break;
        case 5:
            FOR (int j = 0; j < 51200; ++j)
                asm volatile ("pause;\n" :::);
            break;
        case 6:
            FOR (int j = 0; j < 61440; ++j)
                asm volatile ("pause;\n" :::);
            break;
        case 7:
            FOR (int j = 0; j < 71680; ++j)
                asm volatile ("pause;\n" :::);
            break;
        case 8:
            FOR (int j = 0; j < 81920; ++j)
                asm volatile ("pause;\n" :::);
            break;
        case 9:
            FOR (int j = 0; j < 92160; ++j)
                asm volatile ("pause;\n" :::);
            break;
        case 10:
            FOR (int j = 0; j < 102400; ++j)
                asm volatile ("pause;\n" :::);
            break;
        default:
            FOR (int j = 0; j < 1'024'000; ++j)
                asm volatile ("pause;\n" :::);
            break;
    }
}

static
double lsq(unsigned deg, unsigned n, double const* x, double const* y, double* a) {
    std::vector<long double> sx(2 * deg + 1);
    std::vector<long double> sy(deg + 1);

    std::fill(sx.begin(), sx.end(), 0.0L);
    std::fill(sy.begin(), sy.end(), 0.0L);

    for (unsigned i = 0; i < n; ++i) {
    }

    return 0;
}


static
void measure_cpu_frequency() {
    std::vector<double> iv;
    std::vector<double> y;

    std::vector<int64_t> x;
    auto t0 = std::chrono::high_resolution_clock::now();

    for (int l = 0; l <= 10; ++l) {
        uint64_t v[84] = { 0 };
        for (int i = 0; i < 84; ++i) {
            load(l);
            auto v1 = x_rdtscp1();
            v[i] = v1;
        }

        x.clear();
        for (int i = 0; i < 83; ++i) { x.push_back(v[i + 1] - v[i]); }
        std::sort(x.begin(), x.end());
        int64_t tsc = x[x.size() / 2];

        for (int i = 0; i < 84; ++i) {
            load(l);
            auto t1 = std::chrono::high_resolution_clock::now();
            v[i] = (t1 - t0).count();
        }

        x.clear();
        for (int i = 0; i < 83; ++i) { x.push_back(v[i + 1] - v[i]); }
        std::sort(x.begin(), x.end());
        int64_t chr = x[x.size() / 2];
        iv.push_back(l * 10 * 1024);
        y.push_back(tsc);

        fprintf(stdout, "data, %d, %ld, %ld\n", l * 10 * 1024, tsc, chr);
   }
   double a[2] = { 0.0 };
   lsq(1, iv.size(), iv.data(), y.data(), a);
}

int main() {
    measure_cpu_frequency();
    return 0;
}
