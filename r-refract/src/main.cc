#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <stdexcept>

#include "canvas.hh"
#include "inlines.hpp"

using Random = std::mt19937_64;

static inline constexpr
unsigned parity(uint64_t w) {
    unsigned nb = 0;
    while (w > 0) { w &= (w - 1); ++nb; }
    return (nb & 1);
}


static
void benchmark1(int width, int height, unsigned repeats, Random& rng) {
    Canvas canvas(width, height, true);
    int w_2 = width - 2;
    int w_1 = width - 1;
    uint8_t ca = 0;

    canvas.set_fg_color(15);

    auto Initialize = [&](unsigned x, Canvas& canvas) {
        ca = x;
    };

    auto Load = [&](Canvas& canvas) {
        canvas.clear();
        canvas.set_fg_color(10);
        canvas.rectangle(0, 0, width - 1, height - 1);
        canvas.set_fg_color(6);
        #if 0
        for (int j = 1; j < w_1; ++j) {
            if (parity(rng())) { canvas.pset(j, 1); }
        }
        #endif
        canvas.pset(width / 2, 1); 
        for (int i = 2; i < height - 1; ++i) {
            for (int j = 2; j < w_2; ++j) {
                unsigned z = 0;
                for (int k = -1; k < 2; ++k) {
                    z <<= 1;
                    if (canvas.pget8(j + k, i - 1) == canvas.get_bg_color8()) { ++z; }
                }
                if ((ca >> z) & 1) { canvas.pset(j, i); }
            }
        }
    };

    auto Time = [=](unsigned seq, Canvas& canvas) {
        Initialize(seq, canvas);

        auto t1 = std::chrono::steady_clock::now();
        asm ("\n\tmfence;\n":::"memory");
        Load(canvas);
        asm ("\n\tlfence;\n":::"memory");
        auto t2 = std::chrono::steady_clock::now();

        std::string name = "result/output" + std::to_string(seq) + ".ppm";
        canvas.write_ppm16(name);
        return canvas.get_pixel_count() * 1.0 / (t2 - t1).count(); /// pixel rate
    };

    std::vector<double> timing;
    for (unsigned i = 0; i < repeats; ++i) {
        timing.push_back(Time(i, canvas));
        fputc('.', stderr);
    }
    fputc('\n', stderr);

    std::sort(timing.begin(), timing.end());
    fprintf(stdout, "%lf ns / pixel\n", 1.0 / timing[timing.size() / 2]);
}

int main() try {
    uint64_t seed = x_rdrand64();
    Random prng(seed);

    benchmark1(2560, 1440, 256, prng);

    return 0;
} catch (std::exception const& e) {
    fprintf(stderr, "Exception: %s\n", e.what());
    std::exit(-127);
} /// catch
