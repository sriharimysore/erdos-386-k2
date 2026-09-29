/*
 * Erdős #386, k = 2: all n <= N with C(n,2) a product of consecutive primes.
 * Fast version of block_search.py. Exact integer arithmetic only (unsigned
 * __int128); no floating point anywhere.
 *
 * Blocks p_a * ... * p_{b-1} with product P <= M = N(N-1)/2 are split as:
 *   length 1  : skipped. Lemma: for n >= 4, C(n,2) = (n/2)(n-1) or n((n-1)/2)
 *               with both factors > 1, so it is never prime.
 *   length 2  : (p, nextprime(p)) for all p with p*q <= M, so p <= isqrt(M).
 *               Streamed from a multithreaded segmented sieve.
 *   length >=3: the third-to-last prime is <= icbrt(M), so every such block
 *               lives in a stored list of primes up to icbrt(M) + margin.
 * For each P, test whether D = 1 + 8P is a perfect square: cheap modular
 * quadratic-residue filters first (exact; they only reject true non-squares),
 * then an exact integer square root on the survivors.
 *
 * Build: cc -O3 -march=native -o block_search block_search.c -lpthread
 * Usage: ./block_search N [threads]
 */
#include <inttypes.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned __int128 u128;
typedef uint64_t u64;

#define MARGIN 4000          /* > any prime gap below 1e15 (max ~1500) */
#define SEG (1u << 19)        /* odd numbers per sieve segment */
#define CHUNK ((u64)1 << 30)  /* numbers per work item */

static u64 isqrt64(u64 x) {
    u64 r = 0, bit = (u64)1 << 62;
    while (bit > x) bit >>= 2;
    while (bit) {
        if (x >= r + bit) { x -= r + bit; r = (r >> 1) + bit; } else r >>= 1;
        bit >>= 2;
    }
    return r;
}

static u128 isqrt128(u128 x) {   /* exact floor(sqrt(x)), digit-by-digit */
    u128 r = 0, bit = (u128)1 << 126;
    while (bit > x) bit >>= 2;
    while (bit) {
        if (x >= r + bit) { x -= r + bit; r = (r >> 1) + bit; } else r >>= 1;
        bit >>= 2;
    }
    return r;
}

static u128 icbrt128(u128 x) {   /* exact floor(cbrt(x)) by bisection */
    u128 lo = 0, hi = (u128)1 << 43;
    while (lo < hi) {
        u128 mid = (lo + hi + 1) / 2;
        if (mid * mid * mid <= x) lo = mid; else hi = mid - 1;
    }
    return lo;
}

/* Quadratic-residue filters. D is a square => D mod m is a QR mod m. */
static const u64 MODS[] = {63 * 65 * 11 * 17, 19 * 23 * 29 * 31};
#define NMODS 2
static uint8_t *qr[NMODS];

static void init_qr(void) {
    for (int k = 0; k < NMODS; k++) {
        u64 m = MODS[k];
        qr[k] = calloc(m, 1);
        for (u64 x = 0; x < m; x++) qr[k][(x * x) % m] = 1;
    }
}

static u128 M;       /* N(N-1)/2 */
static u64 N_;
static _Atomic u64 n_filter_pass, n_len2, n_len3p;

/* If P = n(n-1)/2, return n; else 0. pm[k] = P mod MODS[k] (precomputed). */
static u64 tri_root(u128 P, const u64 *pm) {
    for (int k = 0; k < NMODS; k++)
        if (!qr[k][(1 + 8 * pm[k]) % MODS[k]]) return 0;
    atomic_fetch_add_explicit(&n_filter_pass, 1, memory_order_relaxed);
    u128 D = 1 + 8 * P;
    u128 s = isqrt128(D);
    if (s * s != D) return 0;
    return (u64)((1 + s) / 2);
}

/* ---- solutions ---- */
static pthread_mutex_t sol_mu = PTHREAD_MUTEX_INITIALIZER;
typedef struct { u64 n, first, last, len; } sol_t;
static sol_t sols[256];
static int nsols;

static void report(u64 n, u64 first, u64 last, u64 len) {
    pthread_mutex_lock(&sol_mu);
    if (nsols < 256) sols[nsols++] = (sol_t){n, first, last, len};
    pthread_mutex_unlock(&sol_mu);
}

/* ---- simple sieve for stored primes ---- */
static u64 *simple_primes(u64 L, u64 *cnt) {
    uint8_t *c = calloc(L + 1, 1);
    u64 k = 0;
    for (u64 i = 2; i * i <= L; i++)
        if (!c[i]) for (u64 j = i * i; j <= L; j += i) c[j] = 1;
    for (u64 i = 2; i <= L; i++) k += !c[i];
    u64 *ps = malloc(k * sizeof(u64));
    k = 0;
    for (u64 i = 2; i <= L; i++) if (!c[i]) ps[k++] = i;
    free(c);
    *cnt = k;
    return ps;
}

/* ---- length >= 3 blocks (plus the length-2 block starting at 2) ---- */
static void long_blocks(void) {
    u64 C3 = (u64)icbrt128(M) + MARGIN, np;
    u64 *ps = simple_primes(C3, &np);
    for (u64 a = 0; a + 2 < np; a++) {
        u128 P = (u128)ps[a] * ps[a + 1];
        if (a == 0 && P <= M) {       /* block {2,3}; odd p handled by stream */
            u64 pm[NMODS];
            for (int k = 0; k < NMODS; k++) pm[k] = (u64)(P % MODS[k]);
            u64 n = tri_root(P, pm);
            if (n) report(n, ps[a], ps[a + 1], 2);
        }
        if (P * ps[a + 2] > M) break;
        for (u64 b = a + 2;; b++) {
            if (b >= np) { fprintf(stderr, "stored primes too short\n"); exit(1); }
            P *= ps[b];
            if (P > M) break;
            n_len3p++;
            u64 pm[NMODS];
            for (int k = 0; k < NMODS; k++) pm[k] = (u64)(P % MODS[k]);
            u64 n = tri_root(P, pm);
            if (n) report(n, ps[a], ps[b], b - a + 1);
        }
    }
    free(ps);
}

/* ---- length-2 blocks via segmented sieve ---- */
static u64 *base; static u64 nbase;   /* odd primes <= sqrt(S + CHUNK) */
static u64 S;                         /* isqrt(M): p <= S for any pair */
static _Atomic u64 next_chunk;
static u64 nchunks;

/* Sieve odd numbers in [lo, hi) (lo odd); mark composites in buf. */
static void sieve_seg(u64 lo, u64 hi, uint8_t *buf) {
    u64 len = (hi - lo + 1) / 2;
    memset(buf, 1, len);
    for (u64 i = 0; i < nbase; i++) {
        u64 p = base[i];
        if (p * p >= hi) break;
        u64 st = p * p;
        if (st < lo) { st = ((lo + p - 1) / p) * p; if (!(st & 1)) st += p; }
        for (u64 j = (st - lo) / 2; j < len; j += p) buf[j] = 0;
    }
    if (lo == 1) buf[0] = 0;
}

static int check_pair(u64 p, u64 q) {   /* returns 1 if the block was tested */
    u128 P = (u128)p * q;
    if (P > M) return 0;
    u64 pm[NMODS];
    for (int k = 0; k < NMODS; k++)
        pm[k] = (u64)(((u128)(p % MODS[k]) * (q % MODS[k])) % MODS[k]);
    u64 n = tri_root(P, pm);
    if (n) report(n, p, q, 2);
    return 1;
}

static void *worker(void *arg) {
    (void)arg;
    uint8_t *buf = malloc(SEG);
    for (;;) {
        u64 c = atomic_fetch_add(&next_chunk, 1);
        if (c >= nchunks) break;
        u64 lo = 3 + c * CHUNK, hi = lo + CHUNK;      /* starts p in [lo, hi) */
        if (hi > S + 1) hi = S + 1;
        u64 prev = 0, local2 = 0;
        /* sieve [lo, hi + MARGIN) to also find the successor of the last p */
        for (u64 s = lo; s < hi + MARGIN; s += 2 * (u64)SEG) {
            u64 e = s + 2 * (u64)SEG;
            if (e > hi + MARGIN) e = hi + MARGIN;
            sieve_seg(s, e, buf);
            u64 len = (e - s + 1) / 2;
            for (u64 j = 0; j < len; j++) {
                if (!buf[j]) continue;
                u64 q = s + 2 * j;
                if (prev) local2 += check_pair(prev, q);
                if (q >= hi) { prev = 0; goto done; }
                prev = q;
            }
        }
        fprintf(stderr, "no prime found in margin after %" PRIu64 "\n", hi);
        exit(1);
    done:
        atomic_fetch_add(&n_len2, local2);
    }
    free(buf);
    return NULL;
}

static double now(void) {
    struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

static void print_u128(u128 x) {
    char b[50]; int i = 49; b[i] = 0;
    do { b[--i] = '0' + (int)(x % 10); x /= 10; } while (x);
    fputs(b + i, stdout);
}

static int cmp_sol(const void *a, const void *b) {
    u64 x = ((const sol_t *)a)->n, y = ((const sol_t *)b)->n;
    return (x > y) - (x < y);
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s N [threads]\n", argv[0]); return 1; }
    N_ = strtoull(argv[1], 0, 10);
    int T = argc > 2 ? atoi(argv[2]) : 8;
    if (N_ > (u64)10000000000000ULL * 10) { fprintf(stderr, "N too large\n"); return 1; }
    M = (u128)N_ * (N_ - 1) / 2;
    S = (u64)isqrt128(M);
    init_qr();
    double t0 = now();

    long_blocks();
    double t1 = now();

    u64 bl; base = simple_primes(isqrt64(S + MARGIN) + 1, &bl);
    base++; nbase = bl - 1;                    /* drop 2: odd-only sieve */
    nchunks = (S - 3) / CHUNK + 1;
    pthread_t th[64];
    for (int i = 0; i < T; i++) pthread_create(&th[i], 0, worker, 0);
    for (int i = 0; i < T; i++) pthread_join(th[i], 0);
    double t2 = now();

    printf("N = %" PRIu64 "  M = ", N_); print_u128(M); printf("\n");
    printf("isqrt(M) = %" PRIu64 ", icbrt(M) = %" PRIu64 "\n", S, (u64)icbrt128(M));
    printf("blocks: length-2 (odd start) %" PRIu64 ", length>=3 %" PRIu64
           ", passed QR filters %" PRIu64 "\n", n_len2, n_len3p, n_filter_pass);
    printf("time: long blocks %.2fs, length-2 stream %.2fs (%d threads)\n", t1 - t0, t2 - t1, T);
    qsort(sols, nsols, sizeof(sol_t), cmp_sol);
    for (int i = 0; i < nsols; i++)
        printf("  n = %" PRIu64 "  primes %" PRIu64 "..%" PRIu64 " (%" PRIu64 " primes)\n",
               sols[i].n, sols[i].first, sols[i].last, sols[i].len);
    return 0;
}
