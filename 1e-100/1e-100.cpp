#include <array>
#include <cstdio>
#include <vector>

#include <gmp.h>
#include <mpfr.h>

static
void solve(unsigned precision) {
    std::array<mpz_t, 3> pn;
    std::array<mpz_t, 3> qn;
    mpz_t ti;

    for (auto& p: pn) { mpz_init(p); }
    for (auto& q: qn) { mpz_init(q); }
    mpz_init(ti);

    mpfr_t src, x, t, a, b, r;

    mpfr_init2(src, precision);
    mpfr_init2(x, precision);
    mpfr_init2(t, precision);
    mpfr_init2(a, precision);
    mpfr_init2(b, precision);
    mpfr_init2(r, precision);


    mpfr_const_pi(src, MPFR_RNDN);
    mpfr_set(x, src, MPFR_RNDN);

    mpfr_get_z(pn[1], src, MPFR_RNDZ);
    mpfr_set_z(t, pn[1], MPFR_RNDN);
    mpfr_sub(x, x, t, MPFR_RNDN);

    mpz_set_si(pn[0], 1);
    mpz_set_si(qn[0], 0);
    mpz_set_si(qn[1], 1);

    bool expand = true;
    int i = 0;
    while (expand) {
        mpfr_ui_div(x, 1, x, MPFR_RNDN);
        mpfr_get_z(ti, x, MPFR_RNDZ);
        mpfr_set_z(t, ti, MPFR_RNDN);
        mpfr_sub(x, x, t, MPFR_RNDN);

        mpz_mul(pn[2], pn[1], ti);
        mpz_add(pn[2], pn[2], pn[0]);

        mpz_mul(qn[2], qn[1], ti);
        mpz_add(qn[2], qn[2], qn[0]);

        mpz_set(pn[0], pn[1]);
        mpz_set(pn[1], pn[2]);
        mpz_set(qn[0], qn[1]);
        mpz_set(qn[1], qn[2]);

        mpfr_set_z(a, pn[2], MPFR_RNDN);
        mpfr_sin(r, a, MPFR_RNDN);
        mpfr_abs(r, r, MPFR_RNDN);
        mpfr_log10(r, r, MPFR_RNDN);
        long d = mpfr_get_si(r, MPFR_RNDZ);
        if (d < -1000) {
            gmp_printf("\n\np = %Zd\n", pn[2]);
            break;
        }

        mpfr_set_z(a, pn[2], MPFR_RNDN);
        mpfr_set_z(b, qn[2], MPFR_RNDN);
        mpfr_div(r, a, b, MPFR_RNDN);
        mpfr_sub(r, r, src, MPFR_RNDN);
        mpfr_abs(r, r, MPFR_RNDN);
        mpfr_div(r, r, src, MPFR_RNDN);
        mpfr_log2(r, r, MPFR_RNDN);
        long x = mpfr_get_si(r, MPFR_RNDZ);
        if (-x >= precision) { break; }
        i += 1;
    } /// while(...) -- expand loop

    mpfr_clear(r);
    mpfr_clear(a);
    mpfr_clear(b);
    mpfr_clear(t);
    mpfr_clear(x);
    mpfr_clear(src);
    mpz_t one;
    mpz_init(one); mpz_set_ui(one, 1);
    mpz_t m, n;
    mpz_init(m); mpz_init(n);
    mpz_gcdext(one, m, n, pn[2], qn[2]);
    gmp_printf("m, %Zd, p, %Zd, n, %Zd, q, %Zd, =, 1\n", m, pn[2], n, qn[2]);
} /// solve(...)

int main(int argc, const char* argv[]) {
    unsigned precision = 10'000;
    solve(precision);
    return 0;
}

