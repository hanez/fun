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
