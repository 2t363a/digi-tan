#ifndef X_INLINES_HPP
#define X_INLINES_HPP 1

#include <cstdint>

#include <immintrin.h>


static inline
uint64_t x_rdtscp1() {
    uint64_t v = UINT64_C(0xEEEE'EEEE'EEEE'EEEE);
    asm volatile (
        "\n\trdtscp;"
        "\n\tshl $32, %%rdx;"
        "\n\tor %%rax, %%rdx;"
        "\n\tmov %%rdx, %0;\n"
        :"=r"(v)
        :
        :"rax", "rcx", "rdx"
    );
    return v;
}

static inline
uint64_t x_rdtscp2() {
    uint64_t v = UINT64_C(0xEEEE'EEEE'EEEE'EEEE);
    asm volatile (
        "\n\tlfence; rdtscp; lfence"
        "\n\tshl $32, %%rdx;"
        "\n\tor %%rax, %%rdx;"
        "\n\tmov %%rdx, %0;\n"
        :"=r"(v)
        :
        :"rax", "rcx", "rdx"
    );
    return v;
}

static inline
uint64_t x_rdrand64(uint32_t ecx = 10'000) {
    uint64_t v = UINT64_C(0xEEEE'EEEE'EEEE'EEEE);
    asm volatile (
        "\n\tmov %1, %%ecx;"
        "\n\tL%=.begin:"
        "\n\trdrand %%rax;"
        "\n\tjc L%=.success;"
        "\n\tloopne L%=.begin;"
        "\n\tmov $0xCCCC3333CCCC3333, %%rax;"
        "\n\tL%=.success:"
        "\n\tmov %%rax, %0;\n"
        : "=r"(v)
        : "r"(ecx)
        :"rax", "rcx", "rdx"
    );
    return v;
}

static inline
double x_fma3(double a, double b, double c) {
    __m128d ma = _mm_load_sd(&a);
    __m128d mb = _mm_load_sd(&b);
    __m128d mc = _mm_load_sd(&c);
    __m128d my = _mm_fmadd_sd(ma, mb, mc);
    double y = 0.0;
    _mm_store_sd(&y, my);
    return y;
}

#endif /// #ifndef X_INLINES_HPP

