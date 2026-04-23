#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include <stddef.h>

/* ================================================================
   UNIFORM [0,1)
   ================================================================ */
static inline double uniform01_double(uint64_t r) {
    return (double)(r >> 11) * 0x1p-53;
}

static inline float uniform01_float(uint32_t r) {
    return (float)(r >> 9) * 0x1p-24f;
}

/* ================================================================
   ROTATE + existing PRNGs
   ================================================================ */
static inline uint64_t rotl64(uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

static inline uint64_t splitmix64_next(uint64_t *state) {
    uint64_t z = (*state += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

typedef struct { uint64_t s[2]; } xoroshiro128pp_state __attribute__((aligned(16)));
static inline uint64_t xoroshiro128pp_next(xoroshiro128pp_state *state) {
    const uint64_t s0 = state->s[0];
    uint64_t s1 = state->s[1];
    const uint64_t result = rotl64(s0 + s1, 17) + s0;
    s1 ^= s0;
    state->s[0] = rotl64(s0, 49) ^ s1 ^ (s1 << 21);
    state->s[1] = rotl64(s1, 28);
    return result;
}

typedef struct { uint64_t s[4]; } xoshiro256pp_state __attribute__((aligned(32)));
static inline uint64_t xoshiro256pp_next(xoshiro256pp_state *state) {
    const uint64_t result = rotl64(state->s[0] + state->s[3], 23) + state->s[0];
    const uint64_t t = state->s[1] << 17;
    state->s[2] ^= state->s[0]; state->s[3] ^= state->s[1];
    state->s[1] ^= state->s[2]; state->s[0] ^= state->s[3];
    state->s[2] ^= t; state->s[3] = rotl64(state->s[3], 45);
    return result;
}

static inline uint64_t xoshiro256ss_next(xoshiro256pp_state *state) {
    const uint64_t result = rotl64(state->s[1] * 5, 7) * 9;
    const uint64_t t = state->s[1] << 17;
    state->s[2] ^= state->s[0]; state->s[3] ^= state->s[1];
    state->s[1] ^= state->s[2]; state->s[0] ^= state->s[3];
    state->s[2] ^= t; state->s[3] = rotl64(state->s[3], 45);
    return result;
}

static inline uint64_t xorshift64star_next(uint64_t *state) {
    uint64_t x = *state;
    x ^= x >> 12; x ^= x << 25; x ^= x >> 27;
    *state = x;
    return x * 0x2545F4914F6CDD1DULL;
}

static inline uint64_t wyrand_next(uint64_t *state) {
    *state += 0xa0761d6478bd642full;
    __uint128_t t = (__uint128_t)*state * (*state ^ 0xe7037ed1a0b428dbull);
    return (uint64_t)(t >> 64) ^ (uint64_t)t;
}

/* ================================================================
   UNIFORM INTEGERS – Lemire multiply-high
   ================================================================ */
static inline uint64_t uniform_uint64(uint64_t r, uint64_t range) {
    if (range <= 1ULL) return 0ULL;
    return (uint64_t)((__uint128_t)r * range >> 64);
}

static inline int64_t uniform_int64(uint64_t r, int64_t lo, int64_t hi) {
    if (lo == hi) return lo;
    uint64_t range = (uint64_t)(hi - lo) + 1ULL;
    if (range == 0ULL) return lo + (int64_t)(r >> 1);
    return lo + (int64_t)uniform_uint64(r, range);
}

/* ================================================================
   SEEDING HELPERS
   ================================================================ */
static inline void xoroshiro128pp_seed(xoroshiro128pp_state *st, uint64_t seed) {
    uint64_t sm = seed;
    st->s[0] = splitmix64_next(&sm);
    st->s[1] = splitmix64_next(&sm);
}

static inline void xoshiro256pp_seed(xoshiro256pp_state *st, uint64_t seed) {
    uint64_t sm = seed;
    for (int i = 0; i < 4; ++i)
        st->s[i] = splitmix64_next(&sm);
}

#endif /* RANDOM_H */
