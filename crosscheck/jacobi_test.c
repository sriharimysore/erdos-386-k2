/* Adversarial test: the Jacobi routine used by deep_search.c and small_start_scan.c
 * (copied verbatim) against Euler's criterion a^((m-1)/2) mod m, for random a and for
 * every filter prime actually used by both programs. Also checks that every true square
 * passes (jacobi != -1). */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
typedef uint64_t u64; typedef unsigned __int128 u128;
static int jacobi(u64 a, u64 m) {   /* verbatim from deep_search.c */
    int t = 1; a %= m;
    while (a) {
        while (!(a & 1)) { a >>= 1; u64 r = m & 7; if (r == 3 || r == 5) t = -t; }
        u64 tmp = a; a = m; m = tmp;
        if ((a & 3) == 3 && (m & 3) == 3) t = -t;
        a %= m;
    }
    return m == 1 ? t : 0;
}
static u64 powmod(u64 b, u64 e, u64 m) { u64 r = 1; b %= m;
    while (e) { if (e & 1) r = (u128)r * b % m; b = (u128)b * b % m; e >>= 1; } return r; }
static int isprime(u64 n) { if (n < 2) return 0; for (u64 d = 2; d * d <= n; d++) if (n % d == 0) return 0; return 1; }
static u64 rng = 88172645463325252ULL;
static u64 xr(void) { rng ^= rng << 13; rng ^= rng >> 7; rng ^= rng << 17; return rng; }
int main(void) {
    u64 ells[100]; int n = 0;
    for (u64 x = (1ULL << 31) - 1; n < 48; x -= 2) if (isprime(x)) ells[n++] = x;   /* deep_search */
    for (u64 x = (1ULL << 32) - 1; n < 88; x -= 2) if (isprime(x)) ells[n++] = x;   /* small_start_scan */
    for (u64 x = 3; x < 2000; x += 2) if (isprime(x) && n < 100) ells[n++] = x;     /* small primes */
    long bad = 0, tests = 0, sqbad = 0;
    for (int i = 0; i < n; i++) {
        u64 m = ells[i];
        for (int t = 0; t < 200000; t++) {
            u64 a = xr() % m;
            u64 e = powmod(a, (m - 1) / 2, m);
            int want = a == 0 ? 0 : (e == 1 ? 1 : -1);
            if (jacobi(a, m) != want) bad++;
            u64 s = xr() % m; if (jacobi((u128)s * s % m, m) < 0) sqbad++;
            tests++;
        }
    }
    printf("%d moduli, %ld tests: %ld Jacobi/Euler mismatches, %ld squares wrongly rejected\n",
           n, tests, bad, sqbad);
    return bad || sqbad;
}
