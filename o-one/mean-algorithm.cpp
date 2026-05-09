#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdint>
#include <chrono>
#include <random>
#include <vector>

#include "inlines.hpp"
#include "x-random.hpp"

using Quad = _Float128;

static
double mean(unsigned n, double const* data) {
    std::vector<double> temp(n);

    std::copy(data, data + n, temp.begin());
    auto AbsCompare = [=](double const& a, double const& b) {
        return (std::fabs(a) < std::fabs(b));
    };
    std::sort(temp.begin(), temp.end(), AbsCompare);

    double s10 = 0.0l;
    double s1 = 0.0L;
    Quad q_s1 = 0.0L;
    for (unsigned i = 0; i < n; ++i) { q_s1 += data[i]; s10 += data[i]; s1 += temp[i]; }

    long double ref_s1 = static_cast<long double>(q_s1);
    double rel_s10 = std::fabs((ref_s1 - s10) / ref_s1);
    double rel_s1 = std::fabs((ref_s1 - s1) / ref_s1);

    static int s10_better = 0;
    static int s1_better = 0;
    static int total = 0;
    if (rel_s1 < rel_s10) { s1_better += 1; }
    else if (rel_s10 < rel_s1) { s10_better += 1; }
    total += 1;
    if (rel_s10 != 0 or rel_s1 != 0)
        fprintf(stderr, "s1, %8d, s10, %8d, total, %8d, s1 / total, %6.4lf, s10 / total, %6.4lf\n",
                s1_better, s10_better, total, s1_better * 1.0 / total, s10_better * 1.0/ total);
    return 0;
}

static
void test(unsigned n, Xoro256& rng) {
    std::vector<double> data;
    for (unsigned i = 0; i < n; ++i) { data.push_back(rng.u(0, +0x1.0p+72)); }
    mean(data.size(), data.data());
}

int main() {
    auto rng = Xoro256();
    while (1) {
        test(1'000'000, rng);
    }
    return 0;
}
