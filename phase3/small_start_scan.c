/*
 * Small-start scan: for every start prime s < 100 and every end prime r <= R (R < 2^32),
 * test whether P = s * nextprime(s) * ... * r is a triangular number C(n,2),
 * i.e. whether 1 + 8P is a perfect square. There is no bound on n. The blocks are far too
 * large to compute exactly (up to ~10^(10^9)), so we use NF exact QR filters modulo
 * primes ell in (R, 2^32). No block prime can equal a filter prime, so 1 + 8P mod ell
 * is a uniformly "random" residue, and a non-square is rejected with probability ~1/2
 * per filter. Filters are sound: a real square always passes all of them.
 * Survivors are printed. Blocks of <= 2000 primes are rechecked exactly by verify.py.
 *
 * Build: cc -O3 -mcpu=native -o small_start_scan small_start_scan.c -lpthread
 * Usage: ./small_start_scan R [threads]
 */
#include <inttypes.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint64_t u64;
#define NF 40
#define SEG ((u64)1 << 22)

static u64 R;
static u64 ell[NF];
static u64 base[7000]; static int nbase;
static const u64 STARTS[] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97};
#define NSTARTS 25
static _Atomic int next_start;
static pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;

static int is_prime_u64(u64 n) {
    if (n < 2) return 0;
    for (u64 d = 2; d * d <= n; d++) if (n % d == 0) return 0;
    return 1;
}
static int jacobi(u64 a, u64 m) {
    int t = 1; a %= m;
    while (a) {
        while (!(a & 1)) { a >>= 1; u64 r = m & 7; if (r == 3 || r == 5) t = -t; }
        u64 tmp = a; a = m; m = tmp;
        if ((a & 3) == 3 && (m & 3) == 3) t = -t;
        a %= m;
    }
    return m == 1 ? t : 0;
}

static void *worker(void *arg) {
    (void)arg;
    uint8_t *buf = malloc(SEG);
    for (;;) {
        int si = atomic_fetch_add(&next_start, 1);
        if (si >= NSTARTS) break;
        u64 s = STARTS[si], res[NF], k = 0, surv = 0;
        for (int j = 0; j < NF; j++) res[j] = 1;
        /* stream primes q in [s, R]: handle 2 separately, then odd segments */
        u64 q = s;
        if (s == 2) {
            for (int j = 0; j < NF; j++) res[j] = 2 % ell[j];
            k = 1; q = 3;
        }
        for (u64 lo = q | 1; lo <= R; lo += 2 * SEG) {
            u64 hi = lo + 2 * SEG; if (hi > R + 1) hi = R + 1;
            u64 len = (hi - lo + 1) / 2;
            memset(buf, 1, len);
            for (int i = 1; i < nbase; i++) {
                u64 p = base[i]; if (p * p >= hi) break;
                u64 st = p * p;
                if (st < lo) { st = ((lo + p - 1) / p) * p; if (!(st & 1)) st += p; }
                for (u64 j = (st - lo) / 2; j < len; j += p) buf[j] = 0;
            }
            if (lo == 1) buf[0] = 0;
            for (u64 j = 0; j < len; j++) {
                if (!buf[j]) continue;
                u64 r = lo + 2 * j;
                k++;
                for (int f = 0; f < NF; f++) res[f] = res[f] * r % ell[f];
                if (k < 2) continue;
                int pass = 1;
                for (int f = 0; f < NF && pass; f++)
                    if (jacobi((1 + 8 * res[f]) % ell[f], ell[f]) < 0) pass = 0;
                if (pass) {
                    surv++;
                    pthread_mutex_lock(&mu);
                    printf("SURVIVOR %" PRIu64 " %" PRIu64 "\n", s, k);
                    fflush(stdout);
                    pthread_mutex_unlock(&mu);
                }
            }
        }
        pthread_mutex_lock(&mu);
        printf("START %" PRIu64 ": %" PRIu64 " blocks (lengths 2..%" PRIu64 "), %" PRIu64 " survivors\n",
               s, k - 1, k, surv);
        fflush(stdout);
        pthread_mutex_unlock(&mu);
    }
    free(buf);
    return NULL;
}

int main(int argc, char **argv) {
    R = strtoull(argv[1], 0, 10);
    int T = argc > 2 ? atoi(argv[2]) : 8;
    if (R >= (1ULL << 32) - 1000000) { fprintf(stderr, "R must be < 2^32 - 1e6\n"); return 1; }
    /* filter primes: the NF largest primes below 2^32. All exceed R, so no block prime is one. */
    int j = 0;
    for (u64 x = (1ULL << 32) - 1; j < NF; x -= 2) if (is_prime_u64(x)) ell[j++] = x;
    if (ell[NF - 1] <= R) { fprintf(stderr, "filter primes not above R\n"); return 1; }
    /* base primes up to 2^16 */
    for (u64 x = 2; x < 65536; x++) if (is_prime_u64(x)) base[nbase++] = x;
    printf("small-start scan: starts < 100, end primes r <= %" PRIu64 ", %d filters in (%" PRIu64 ", 2^32)\n",
           R, NF, ell[NF - 1]);
    pthread_t th[64];
    for (int i = 0; i < T; i++) pthread_create(&th[i], 0, worker, 0);
    for (int i = 0; i < T; i++) pthread_join(th[i], 0);
    printf("DONE\n");
    return 0;
}
