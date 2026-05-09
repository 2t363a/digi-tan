#ifndef X_RANDOM_HPP
#define X_RANDOM_HPP 1

#include <cstdint>

/// for RDRAND/RDSEED
#include "inlines.hpp"

/// Freehand C++ implementation following
/// https://prng.di.unimi.it/xoshiro256starstar.c


class Xoro256 {

static inline constexpr
uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }

static inline constexpr
uint64_t next_splitmix64(uint64_t state) {
    uint64_t result = (state += 0x9E3779B97F4A7C15);
    result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
    result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
    return result ^ (result >> 31);
}

public:
    constexpr
    Xoro256(uint64_t seed = 0) {
        m_seed = (seed != 0) ? seed : x_rdrand64();
        reseed(m_seed);
    }

    constexpr
    uint64_t operator()() { return next(); }

    uint64_t get_seed() const { return m_seed; }
    void set_seed(uint64_t seed) { m_seed = seed; reseed(m_seed); }

    double u(double a = 0.0, double b = 1.0) { return x_fma3(next() * 0x1p-64, b - a, a); }

private:
    constexpr
    void reseed(uint64_t seed) {
        uint64_t s = seed;
        m_state[0] = s;
        for (unsigned i = 1; i < 4; ++i) { 
            s = next_splitmix64(s);
            m_state[i] = s;
        }
    } /// reseed

    constexpr
    void jump128() {
        constexpr uint64_t kJump[] = {
            UINT64_C(0x180ec6d33cfd0aba),
            UINT64_C(0xd5a61266f0c9392c),
            UINT64_C(0xa9582618e03fc9aa),
            UINT64_C(0x39abdc4529b1661c)
        };

        uint64_t s0 = 0;
        uint64_t s1 = 0;
        uint64_t s2 = 0;
        uint64_t s3 = 0;
        for(unsigned i = 0; i < sizeof(kJump)/ sizeof(kJump[0]); i++)
            for(unsigned b = 0; b < 64; b++) {
                if (kJump[i] & UINT64_C(1) << b) {
                    s0 ^= m_state[0];
                    s1 ^= m_state[1];
                    s2 ^= m_state[2];
                    s3 ^= m_state[3];
                }
                next();
            }

        m_state[0] = s0;
        m_state[1] = s1;
        m_state[2] = s2;
        m_state[3] = s3;
    } /// jump128(...)

    constexpr
    uint64_t next() {
        uint64_t ret = rotl(m_state[1] * 5, 7) * 9;
        uint64_t t = m_state[1] << 17;

        m_state[2] ^= m_state[0];
        m_state[3] ^= m_state[1];
        m_state[1] ^= m_state[2];
        m_state[0] ^= m_state[3];

        m_state[2] ^= t;
        m_state[3] = rotl(m_state[3], 45);

        return ret;
    } /// next()

    uint64_t m_seed = 0; /// remember seed
    uint64_t m_state[4] = { 0, 0, 0, 0 }; /// state
}; /// class Xoro256

#endif /// #ifndef X_RANDOM_HPP
