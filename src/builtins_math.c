/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/**
* @file builtins_math.c
 * @brief Implements built-in math functions for the Fun VM.
 *
 * This file provides helper functions for mathematical operations, including:
 * - Minimum, maximum, and clamping.
 * - Absolute value and exponentiation.
 * - Random number generation.
 *
 * Functions:
 * - `bm_min`: Returns the smaller of two integers.
 * - `bm_max`: Returns the larger of two integers.
 * - `bm_clamp`: Clamps a value between a lower and upper bound.
 * - `bm_abs`: Returns the absolute value of an integer.
 * - `bm_pow`: Computes the power of an integer.
 * - `bm_random_seed`: Seeds the random number generator.
 * - `bm_random_int`: Generates a random integer within a range.
 *
 * Example:
 * ```c
 * int64_t min = bm_min(10, 20); // 10
 * int64_t rand = bm_random_int(1, 100); // Random number between 1 and 99
 * ```
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */


#include <stdint.h>
#include <stdlib.h>

/* Math built-ins (pure helpers) */

int64_t bm_min(int64_t a, int64_t b) { return a < b ? a : b; }
int64_t bm_max(int64_t a, int64_t b) { return a > b ? a : b; }

int64_t bm_clamp(int64_t x, int64_t lo, int64_t hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

int64_t bm_abs(int64_t x) { return x < 0 ? -x : x; }

int64_t bm_pow(int64_t base, int64_t exp) {
    if (exp < 0) return 0; /* integer pow: negative exponent -> 0 */
    int64_t res = 1;
    int64_t b = base;
    int64_t e = exp;
    while (e > 0) {
        if (e & 1) res *= b;
        b *= b;
        e >>= 1;
    }
    return res;
}

/* RNG: thin wrappers over libc's srand/rand for now */
void bm_random_seed(unsigned int seed) { srand(seed); }

int64_t bm_random_int(int64_t lo, int64_t hi) {
    if (hi <= lo) return lo;
    int64_t span = hi - lo;
    return lo + (rand() % span);
}
