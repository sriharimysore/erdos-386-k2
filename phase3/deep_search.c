/*
 * Deep search for Erdős #386 (k = 2): every block of consecutive primes with start
 * p < X0 = 10,726,905,041 and product <= M, where M < 2^L. Proofs are in LEMMA.md.
 *
 * Phase A (multithreaded segmented sieve over [3, X0]): each start p >= 41 is either
 *   EXCLUDED by Lemma 2 at length k0 = ceil(L/e) - 1 (e = floor(log2 p)), or written
 *   to the enumeration list.
 * Phase B: every start in the list, plus all starts p < 41, is enumerated. For each
 *   length 2 <= k < ceil(L/e), P = block product is tested with NF exact QR filters
 *   (1 + 8P must be a square mod each prime ell). Survivors are printed as
 *   "SURVIVOR a_start_prime k" and must be re-checked with exact big integers
 *   (verify.py). Integer arithmetic only.
 *
 * Build: cc -O3 -mcpu=native -o deep_search deep_search.c -lpthread
 * Usage: ./deep_search L [threads]
 */
#include <inttypes.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef uint64_t u64;
typedef unsigned __int128 u128;

static const u64 X0 = 10726905041ULL;
#define CH ((u64)1 << 25)        /* numbers per Phase A work item */
#define LA ((u64)4000000)        /* sieve lookahead past the chunk (asserted) */
#define NF 48                    /* number of QR filter primes */

static u64 L;                    /* M < 2^L */
static u64 Xmax;                 /* = X0 except in tests */
static int T = 8;
static int nolemma = 0;           /* NOLEMMA=1 disables Lemma 2 (validation) */

static double now(void) {
    struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}
static int ilog2(u64 x) { return 63 - __builtin_clzll(x); }
static u64 kfix = 0;             /* block-length mode: test all lengths k <= kfix */
static u64 kcap(u64 p) {         /* lengths tested are 2 <= k < kcap(p) */
    if (kfix) return kfix + 1;
    u64 e = ilog2(p); return (L + e - 1) / e;   /* K in Lemma 3 */
}

/* ---------- sieve helpers ---------- */
static u64 *base; static u64 nbase;
static u64 *simple_primes(u64 lim, u64 *cnt) {
    uint8_t *c = calloc(lim + 1, 1); u64 k = 0;
    for (u64 i = 2; i * i <= lim; i++) if (!c[i]) for (u64 j = i * i; j <= lim; j += i) c[j] = 1;
    for (u64 i = 2; i <= lim; i++) k += !c[i];
    u64 *ps = malloc(k * sizeof(u64)); k = 0;
    for (u64 i = 2; i <= lim; i++) if (!c[i]) ps[k++] = i;
    free(c); *cnt = k; return ps;
}
/* all primes in [lo, hi), lo odd >= 3, appended to out; returns count */
static u64 seg_primes(u64 lo, u64 hi, uint8_t *buf, u64 *out) {
    u64 len = (hi - lo + 1) / 2, k = 0;
    memset(buf, 1, len);
    for (u64 i = 1; i < nbase; i++) {               /* base[0] = 2 skipped */
        u64 p = base[i]; if (p * p >= hi) break;
        u64 st = p * p;
        if (st < lo) { st = ((lo + p - 1) / p) * p; if (!(st & 1)) st += p; }
        for (u64 j = (st - lo) / 2; j < len; j += p) buf[j] = 0;
    }
    for (u64 j = 0; j < len; j++) if (buf[j]) out[k++] = lo + 2 * j;
    return k;
}

/* ---------- Phase A ---------- */
static _Atomic u64 next_chunk; static u64 nchunks;
static _Atomic u64 n_excluded, n_listed;
static pthread_mutex_t list_mu = PTHREAD_MUTEX_INITIALIZER;
static u64 *elist; static u64 ecount, ecap;

static void *phaseA(void *arg) {
    (void)arg;
    uint8_t *buf = malloc((CH + LA) / 2 + 16);
    u64 *ps = malloc(((CH + LA) / 2 + 16) * sizeof(u64));
    u64 nloc = 0, capl = (u64)1 << 20; u64 *loc = malloc(capl * sizeof(u64));
    for (;;) {
        u64 c = atomic_fetch_add(&next_chunk, 1);
        if (c >= nchunks) break;
        u64 lo = 3 + c * CH, hi = lo + CH;
        if (hi > Xmax + 1) hi = Xmax + 1;              /* starts p in [lo, hi) */
        u64 np = seg_primes(lo, hi + LA, buf, ps);
        u64 ex = 0;
        for (u64 i = 0; i < np && ps[i] < hi; i++) {
            u64 p = ps[i];
            if (p < 41) continue;                      /* enumerated in Phase B */
            if (nolemma) goto list;
            u64 k0 = kcap(p) - 1;
            if (k0 < 2) { ex++; continue; }            /* only length-1 blocks fit */
            if (i + k0 - 1 >= np) { fprintf(stderr, "lookahead too short at p=%" PRIu64 "\n", p); exit(1); }
            u64 r = ps[i + k0 - 1], h = k0 / 2;
            if ((u128)100 * h * (r - p) < (u128)68 * p) { ex++; continue; }   /* Lemma 2 */
        list:
            if (nloc == capl) { capl *= 2; loc = realloc(loc, capl * sizeof(u64)); }
            loc[nloc++] = p;
        }
        atomic_fetch_add(&n_excluded, ex);
        if (nloc) {
            pthread_mutex_lock(&list_mu);
            if (ecount + nloc > ecap) { ecap = 2 * (ecount + nloc); elist = realloc(elist, ecap * sizeof(u64)); }
            memcpy(elist + ecount, loc, nloc * sizeof(u64)); ecount += nloc;
            pthread_mutex_unlock(&list_mu);
            atomic_fetch_add(&n_listed, nloc); nloc = 0;
        }
    }
    free(buf); free(ps); free(loc);
    return NULL;
}

/* ---------- Phase B ---------- */
static u64 ell[NF];
static u64 *P; static u64 nP;     /* all primes up to the needed bound */
static u64 *starts; static u64 nstarts;
static _Atomic u64 next_start, n_tested, n_skipped_lemma;
static pthread_mutex_t out_mu = PTHREAD_MUTEX_INITIALIZER;

static int is_prime_u64(u64 n) {
    if (n < 2) return 0;
    for (u64 d = 2; d * d <= n; d++) if (n % d == 0) return 0;
    return 1;
}
/* Legendre symbol (a / m), m an odd prime, via the Jacobi algorithm. Returns -1, 0, 1. */
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
static u64 idx_of(u64 p) {             /* index of prime p in P[] (binary search) */
    u64 lo = 0, hi = nP;
    while (lo < hi) { u64 mid = (lo + hi) / 2; if (P[mid] < p) lo = mid + 1; else hi = mid; }
    if (lo >= nP || P[lo] != p) { fprintf(stderr, "prime %" PRIu64 " not in table\n", p); exit(1); }
    return lo;
}
static void *phaseB(void *arg) {
    (void)arg;
    for (;;) {
        u64 s = atomic_fetch_add(&next_start, 1);
        if (s >= nstarts) break;
        u64 p = starts[s], a = idx_of(p), K = kcap(p);
        if (a + K > nP) { fprintf(stderr, "prime table too short for p=%" PRIu64 "\n", p); exit(1); }
        u64 res[NF]; for (int j = 0; j < NF; j++) res[j] = 1;
        u64 tested = 0, skipped = 0;
        for (u64 k = 1; k < K; k++) {
            u64 q = P[a + k - 1];              /* last prime of the length-k block */
            if (q >= ell[0]) { fprintf(stderr, "block prime >= filter prime\n"); exit(1); }
            for (int j = 0; j < NF; j++) res[j] = res[j] * q % ell[j];   /* < 2^62 */
            if (k < 2) continue;                   /* Lemma 0 */
            if (!nolemma && p >= 41 && (u128)100 * (k / 2) * (q - p) < (u128)68 * p) { skipped++; continue; }
            tested++;
            int pass = 1;
            for (int j = 0; j < NF && pass; j++)
                if (jacobi((1 + 8 * res[j]) % ell[j], ell[j]) < 0) pass = 0;
            if (pass) {
                pthread_mutex_lock(&out_mu);
                printf("SURVIVOR %" PRIu64 " %" PRIu64 "\n", p, k);
                fflush(stdout);
                pthread_mutex_unlock(&out_mu);
            }
        }
        atomic_fetch_add(&n_tested, tested);
        atomic_fetch_add(&n_skipped_lemma, skipped);
    }
    return NULL;
}

static int cmp64(const void *x, const void *y) {
    u64 a = *(const u64 *)x, b = *(const u64 *)y; return (a > b) - (a < b);
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s L [threads] [Xmax]\n", argv[0]); return 1; }
    if (argv[1][0] == 'k') { kfix = strtoull(argv[1] + 1, 0, 10); L = 0; }
    else L = strtoull(argv[1], 0, 10);
    nolemma = getenv("NOLEMMA") != NULL;
    if (nolemma) printf("NOLEMMA: Lemma 2 pruning disabled\n");
    if (argc > 2) T = atoi(argv[2]);
    Xmax = argc > 3 ? strtoull(argv[3], 0, 10) : X0;       /* < X0 only for testing */
    if (kfix) printf("block-length mode: all lengths k <= %" PRIu64 ", starts p <= %" PRIu64 ", threads %d\n", kfix, Xmax, T);
    else printf("L = %" PRIu64 " (M < 2^L), starts p <= %" PRIu64 ", threads %d\n", L, Xmax, T);

    /* filter primes: the NF largest primes below 2^31 */
    for (u64 x = (1ULL << 31) - 1, j = 0; j < NF; x -= 2) if (is_prime_u64(x)) ell[j++] = x;
    /* make ell[0] the smallest (used in the block-prime check) */
    for (int i = 0; i < NF / 2; i++) { u64 t = ell[i]; ell[i] = ell[NF - 1 - i]; ell[NF - 1 - i] = t; }

    double t0 = now();
    base = simple_primes(120000, &nbase);          /* sqrt(X0 + CH + LA) < 104000 */
    nchunks = (Xmax - 3) / CH + 1;
    pthread_t th[64];
    /* Phase A over [3, Xmax] */
    for (int i = 0; i < T; i++) pthread_create(&th[i], 0, phaseA, 0);
    for (int i = 0; i < T; i++) pthread_join(th[i], 0);
    double t1 = now();
    printf("Phase A: %" PRIu64 " starts excluded by Lemma 2, %" PRIu64 " listed; %.1fs\n",
           (u64)n_excluded, (u64)n_listed, t1 - t0);

    /* Phase B starts: primes < 41 plus the list */
    qsort(elist, ecount, sizeof(u64), cmp64);
    u64 small[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    nstarts = 12 + ecount;
    starts = malloc(nstarts * sizeof(u64));
    memcpy(starts, small, sizeof small);
    memcpy(starts + 12, elist, ecount * sizeof(u64));
    u64 maxstart = ecount ? elist[ecount - 1] : 37;
    /* need P[a + K - 1] for each start: sieve generously and assert in phaseB */
    u64 need = maxstart + 40 * (kfix ? 40 * kfix : L) + 1000000;
    P = simple_primes(need, &nP);
    printf("Phase B: %" PRIu64 " starts, max start %" PRIu64 ", prime table to %" PRIu64 "\n",
           nstarts, maxstart, need);
    for (int i = 0; i < T; i++) pthread_create(&th[i], 0, phaseB, 0);
    for (int i = 0; i < T; i++) pthread_join(th[i], 0);
    double t2 = now();
    printf("Phase B: %" PRIu64 " blocks tested, %" PRIu64 " skipped by Lemma 2; %.1fs\n",
           (u64)n_tested, (u64)n_skipped_lemma, t2 - t1);
    printf("DONE\n");
    return 0;
}
