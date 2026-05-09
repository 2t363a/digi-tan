#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "mpfr.h"

#include "inlines.hpp"

static
void calculate_constant(unsigned prec, mpfr_t c) {
    mpfr_init2(c, prec);
    mpfr_const_pi(c, MPFR_RNDN);
    mpfr_ui_div(c, 4, c, MPFR_RNDN);
 //   mpfr_mul_ui(c, c, 2, MPFR_RNDN);
}

static
void expand_ld(unsigned prec, mpfr_t x) {
    mpfr_t rem;
    mpfr_t tmp;

    mpfr_init2(rem, prec);
    mpfr_init2(tmp, prec);

    mpfr_set(rem, x, MPFR_RNDN);
    long double v = mpfr_get_ld(rem, MPFR_RNDN);
    do {
        fprintf(stdout, "%32.20La\n", v);
        mpfr_set_ld(tmp, v, MPFR_RNDN);
        mpfr_sub(rem, rem, tmp, MPFR_RNDN);
        v = mpfr_get_ld(rem, MPFR_RNDN);
    } while (std::fabs(v) > 0.0L);

    mpfr_clear(tmp);
    mpfr_clear(rem);
}

int main(int argc, const char* argv[]) try {
    unsigned prec = 1024;
    mpfr_t c;

    calculate_constant(prec, c);
    expand_ld(prec, c);
    mpfr_clear(c);

    return 0;
} catch (std::exception const& e) {
    fprintf(stderr, "std::exception: %s\n", e.what());
    std::exit(127);
}
