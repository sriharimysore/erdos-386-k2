# Erdős #386, k = 2: bounded verification log

Target statement (google-deepmind/formal-conjectures, `Erdos386.erdos_386.variants.two`):

    answer(sorry) ↔ ∃ᶠ n in .atTop,
      2 ≤ n - 2 ∧ ∃ p q : ℕ, n.choose 2 = ∏ i ∈ .Ico p q, nth Nat.Prime i

**This is a bounded verification only. It does not solve the problem.** The problem
(infinitely many n?) cannot be settled by finite computation.

## Phase 1: computational search (Python + C)

### Algorithm

We loop over blocks of consecutive primes, not over n. For each start index a, form
P = p_a · p_{a+1} · … and keep extending while P ≤ M := N(N−1)/2. For each P:

    P = n(n−1)/2  ⇔  8P + 1 = 4n² − 4n + 1 = (2n−1)²

So P is a triangular number C(n,2) iff D = 1 + 8P is a perfect square s². Since D is odd,
s is odd and n = (1+s)/2 is an integer. We test this with exact `isqrt` (Python) or an
exact bit-by-bit 128-bit integer square root (C). No floats are used anywhere.

Why this covers every n ≤ N: C(n,2) ≤ M ⇔ n ≤ N. So every solution n ≤ N corresponds to a
block with product ≤ M, and each such block is enumerated. The only exceptions are length-1
blocks with a large prime (see below). Empty blocks (p ≥ q) give product 1 = C(2,2), which
the condition n ≥ 4 excludes.

Which primes are needed:
- Length 1: C(n,2) is never prime for n ≥ 4, because C(n,2) = (n/2)(n−1) or n·((n−1)/2)
  with both factors > 1. The Python version still checks length-1 blocks for primes it has
  already sieved. The C version skips them.
- Length ≥ 2: the second-to-last prime satisfies p_{b−2}² < p_{b−2}·p_{b−1} ≤ M, so
  p_{b−2} ≤ isqrt(M). The last prime is therefore at most the first prime above isqrt(M).
  Sieving to isqrt(M) + margin is enough. The code asserts this rather than assuming it.
- Length ≥ 3: the third-to-last prime is ≤ icbrt(M). The C version stores primes only up to
  icbrt(M) + margin for these blocks, and streams length-2 blocks (p, nextprime p) from a
  segmented sieve.

Why it beats looping on n: looping on n means factoring every C(n,2) for n ≤ N, which takes
N factorizations (or a sieve to N plus N factor-and-check passes). The block search visits
only about π(√M) ≈ N/(√2·ln N) candidates, since almost all blocks with product ≤ M have
length 2. Each candidate costs O(1) multiplications and one square test, with no
factoring. The work needed:
- sieving to √M ≈ 0.707·N: O(N log log N) time. The segmented sieve in C uses O(√N) memory.
- ~N/ln N length-2 blocks, plus O(M^{1/3}) = O(N^{2/3}) blocks of length ≥ 3.

Asymptotically the sieve dominates, so the whole search is roughly linear in N but with a
very small constant: no factoring, and cheap modular quadratic-residue filters in C reject
99.97% of candidates before the exact isqrt. The filters are exact: they reject only true
non-squares. Measured: ~15 s for N = 10¹¹ on 8 cores (M2).

### Files
- `phase1/block_search.py`: pure-Python reference (exact `math.isqrt`).
- `phase1/brute_check.py`: independent check that loops over n and factors C(n,2) with an
  SPF sieve. Shares no code with the block search.
- `phase1/block_search.c`: fast version (u128, segmented sieve, pthreads).
  Build: `cc -O3 -mcpu=native -o block_search block_search.c -lpthread`

### Results (2026-09-29, Apple M2, 8 cores)

| N | method | runtime | blocks tested (len ≥ 2) | solutions n ≥ 4 |
|---|---|---|---|---|
| 10⁶ | brute_check.py (loop on n) | 1.0 s | n/a | 4, 6, 15, 21, 715 |
| 10⁷ | brute_check.py (loop on n) | (not timed) | n/a | 4, 6, 15, 21, 715 |
| 10⁶ | block_search.py | 0.05 s | 58,321 | 4, 6, 15, 21, 715 |
| 10⁹ | block_search.py | 22.6 s | 36,669,061 | 4, 6, 15, 21, 715 |
| 10⁶ | block_search.c | <0.01 s | 58,321 | 4, 6, 15, 21, 715 |
| 10⁹ | block_search.c | 0.7 s | 36,669,061 | 4, 6, 15, 21, 715 |
| 10¹⁰ | block_search.c | 1.5 s | 327,211,540 | 4, 6, 15, 21, 715 |
| 10¹¹ | block_search.c | 15.1 s | 2,955,274,669 | 4, 6, 15, 21, 715 |
| 10¹² | block_search.c | 160 s | 26,949,306,670 | 4, 6, 15, 21, 715 |

Cross-validation: at N = 10⁶ and 10⁹, Python and C enumerate identical counts of length-2
and length-≥3 blocks (for 10⁹: 36,601,881 and 67,180). All three implementations agree on
the solution set where they overlap. n = 3 (C(3,2) = 3) also appears, but `2 ≤ n − 2`
excludes it.

**Correction to the starting brief:** n = 715 is a solution:
C(715,2) = 255255 = 3·5·7·11·13·17 (715 = 5·11·13, 714 = 2·3·7·17). erdosproblems.com/386
lists it among the known values 4, 6, 15, 21, 715. The Phase 2 target set must be
{4, 6, 15, 21, 715}, not {4, 6, 15, 21}.

### Structural patterns in the solutions

Write n(n−1)/2 = (E/2)·O, where E is the even one of {n, n−1} and O is the odd one.
gcd(E/2, O) = 1, so the block's primes split into S (dividing E/2) and T (dividing O),
with 2·∏S − ∏T = ±1.

| n | block | n(n−1) | E/2 | O | |S|,|T| |
|---|---|---|---|---|---|
| 4 | 2·3 | 12 | 2 | 3 | 1,1 |
| 6 | 3·5 | 30 = 5# | 3 | 5 | 1,1 |
| 15 | 3·5·7 | 210 = 7# | 7 | 15 = 3·5 | 1,2 |
| 21 | 2·3·5·7 | 420 | 10 = 2·5 | 21 = 3·7 | 2,2 |
| 715 | 3·…·17 | 510510 = 17# | 357 = 3·7·17 | 715 = 5·11·13 | 3,3 |

- Every solution's block starts at 2 or 3.
- Blocks starting at 3 are exactly the cases n(n−1) = p# (a primorial), i.e. two consecutive
  integers whose product is a primorial. This is the classical "714 and 715" question
  (Nelson–Penney–Pomerance, 1974). Blocks starting at 2 are the cases n(n−1) = 2·p#
  (n = 4, 21).
- If the block avoids 2, P is odd, so n ≡ 2, 3 (mod 4) (6, 15, 715). If it contains 2,
  n ≡ 0, 1 (mod 4) (4, 21).
- The split is balanced: the two halves n and n−1 are nearly equal, so the block's primes
  must divide into two sets with products in ratio almost exactly 2. For a block of large,
  close-together primes this forces a long block. That is why only tiny starting primes
  show up.

### Possible speed-up (not used in the verified runs)

Length-2 blocks: if C(n,2) = p·q with p < q consecutive primes and n ≥ 4, then
{E/2, O} = {p, q} and q = 2p ± 1.
- q = 2p + 1 is impossible by Bertrand (prime in (p, 2p], and 2p is not prime).
  This is in Mathlib (`Nat.exists_prime_lt_and_le_two_mul`).
- q = 2p − 1 (n = 2p) needs a prime strictly inside (p, 2p − 1), i.e. the stronger
  Chebyshev form "prime in (m, 2m − 2) for m > 3". I haven't found this in Mathlib yet
  (open issue).
With this, only length-≥3 blocks remain, which need only primes up to about M^{1/3}.
The search would then cost O(N^{2/3}) and could go far beyond 10¹². None of the numbers
above rely on it.

## Open issues
- Phase 2 not started (awaiting approval).
- Next decade: N = 10¹³ would take ~27 min on this machine, 10¹⁴ ~4.5 h. Not run.
- Sanity check on the 10¹² count: 26,944,597,549 odd-start pairs ≈ π(7.07·10¹¹) − 1.
  The estimate x/(ln x − 1.08) gives ≈ 2.70·10¹⁰, which is consistent. Not checked
  against an exact published π value.
- (Resolved) isqrt128 in the C code was unit-tested against Python `math.isqrt` on 800,009
  values (random up to 2¹²⁶, plus s²−1, s², s²+1 around each), with 0 mismatches. The QR
  filters are sound by construction: the table marks every x² mod m, and
  D mod m = (1 + 8·(P mod m)) mod m.
- Stronger Bertrand form for the q = 2p − 1 case (see above), if we want the pruning.


## Phase 3: deep search via the balance lemma (2026-09-29)

Literature check: OEIS A280992 (squarefree triangular numbers that are products of
consecutive primes) says there are no more terms up to the 5,000,000th triangular number,
i.e. n ≤ 5·10⁶. So the Phase 1 bound of 10¹² already extends the published search.
Nelson–Penney–Pomerance searched only the primorial sub-case (n(n−1) = p#) up to the first
3049 primes.

Idea (full proofs in `phase3/LEMMA.md`): n and n−1 differ by 1, so the block's primes must
split into two groups with products in ratio ≈ 2. For a block from p to r with k primes,
this forces (r/p)^⌊k/2⌋ ≥ 2 − 1/p (Lemma 1, the balance lemma). The lemma is sharp:
equality holds at n = 14, where C(14,2) = 91 = 7·13 (not a consecutive block).

Consequences:
- A block starting at a large prime p must be very long (k ≳ √(p/ln p)), so its product is
  enormous.
- Starts p < X₀ = 10,726,905,041 are checked by computer. Each start is either excluded
  by an O(1) exact test (Lemma 2 at the maximal relevant length) or fully enumerated.
- Starts p ≥ X₀ are ruled out analytically using explicit prime-gap theorems.

How far the tail argument reaches (`phase3/tail_bounds.py`), with X₀ = 10,726,905,041:
| theorem | covers N = 10^D for D ≤ |
|---|---|
| Dusart 2010 Prop 6.8 (checked in the arXiv PDF 1002.0442) | 682 |
| Dusart 2018 Cor 5.5 (as quoted in Axler, arXiv 1703.08032) | 9614 |
| Ramaré–Saouter 2003 Thm 3 (as quoted in Axler) | 31310 |

Validation:
- `lemma_stress.py`: Lemma 1 was checked on all 414,792 odd squarefree C(n,2) with
  n ≤ 3·10⁶ (prime factors need not be consecutive). 0 violations.
- `deep_search` at N = 10⁹ (starts up to √M, no analytic tail needed), with and without
  the Lemma 2 pruning (NOLEMMA=1): both give exactly {4, 6, 15, 21, 715}, matching Phase 1.
  With pruning, 48 starts were enumerated. Without it, 36.6M were, with 36.7M blocks tested.
- Phase A coverage: excluded + listed + 12 small starts = 486,570,088 = π(X₀), computed
  independently (`prime_count.py`, Lucy_Hedgehog algorithm).
- The survivors of the 48 modular QR filters are all re-checked with exact isqrt
  (`verify.py`). So far there are 0 false positives.

Results:
| N | runtime | starts enumerated | blocks tested | solutions |
|---|---|---|---|---|
| 10¹⁰⁰ | 32 s | 1,920 | 50,442 | 4, 6, 15, 21, 715 |
| 10⁶⁸² | 33 s | 45,555 | 5,075,043 | 4, 6, 15, 21, 715 |
| 10⁹⁶¹⁴ | 480 s | 4,702,043 | 4,770,517,626 | 4, 6, 15, 21, 715 |

**Claim (conditional only on Dusart 2010 Prop. 6.8, a published theorem):** for 4 ≤ n ≤ 10⁶⁸²,
C(n,2) is a product of consecutive primes only for n ∈ {4, 6, 15, 21, 715}.
Conditional on Dusart 2018 Cor. 5.5 (quoted via Axler, not yet read in the original), the same
holds for n ≤ 10⁹⁶¹⁴ (Phase A coverage 481,868,045 + 4,702,031 + 12 = π(X₀) ✓). The search is
also unconditional up to n ≤ 10¹² (Phase 1).

Trust caveats:
- The computation is plain C/Python, not formally verified.
- The tail uses a published explicit prime-gap theorem.
- Lemmas 1–3 are proved by hand in LEMMA.md. They are not yet machine-checked.

Blocker: the disk has 2.2 GB free, and Mathlib's cache needs ~5–7 GB, so Lean (Phase 2) is
blocked until space is freed.

## Phase 4: block-length theorem and first Lean proofs (2026-09-29)

**Theorem (holds for every n, with no size limit).** If C(n,2) is a product of k consecutive
primes and n ∉ {4, 6, 15, 21, 715}, then k ≥ 137 (assuming Dusart 2010), and k ≥ 1924
(assuming Dusart 2018). Proof: `phase3/LEMMA.md`, "Block-length theorem".
- Computation: `deep_search k136` (20 s) and `deep_search k1923` (138 s, 1.86·10⁹ blocks
  tested). Coverage = π(X₀) in both runs.
- Threshold for large starting primes: `phase3/length_bounds.py`.
- Not proven: anything about long blocks starting at 2 or 3. That includes the open
  "714·715" primorial problem.

**Lean.** `Erdos 386/Balance.lean`: the arithmetic core of the balance lemma is
kernel-checked in Lean 4 core, without Mathlib. The axioms used are propext and Quot.sound.
Mathlib is still not installed: 5.9 GB free, and ~/Library/Caches (9.6 GB) is the user's
to clear.

## Phase 5: small-start scan, no bound on n (2026-09-29)

`phase3/small_start_scan.c`: for every start prime s < 100 and every end prime r ≤ 4·10⁹,
test whether s·nextprime(s)⋯r is a C(n,2). The test uses 40 exact QR filters modulo primes in
(4.29·10⁹, 2³²). None of the filter primes can occur in a block, so every filter stays
effective. The filters are sound: a real square passes all of them.

Result (774 s, 4 threads): 25 × ~1.9·10⁸ ≈ 4.75·10⁹ blocks. The only survivors are the five
known solutions. Coverage: start 2 had 189,961,811 blocks = π(4·10⁹) − 1, and
π(4·10⁹) = 189,961,812 is the known value.

**Theorem (unconditional, computation only).** If C(n,2) is a product of consecutive primes
starting at a prime s < 100 and ending at r ≤ 4·10⁹, then n ∈ {4, 6, 15, 21, 715}. In
particular, n(n−1) = r# (the 714·715 primorial problem) and n(n−1) = 2·r# have no other
solutions for r ≤ 4·10⁹, i.e. through the first ~1.9·10⁸ primes. Nelson–Penney–Pomerance
searched the first 3049.
Validation: the same program at r ≤ 10⁶ rediscovers exactly {4, 6, 15, 21, 715}.

## Phase 6: new-toolkit attempt, idempotents and Riesz products (2026-09-29)

**Reformulation.** n is a solution iff n is the smallest nontrivial idempotent-type residue
(n(n−1) ≡ 0 mod 2P) and λ := n(n−1)/(2P) = 1. There are 2^k such residues mod 2P (one per
split of the block's primes), with CRT basis b_q = (m/q)·c_q, where c_q = (m/q)⁻¹ mod q.

**Experiment** (`phase6/idempotents.py`, starts 2 and 3, r ≤ 97, exhaustive over 2^k):
min λ tracks the random-model prediction P/(2·4^k) to within ~2 orders of magnitude, up to
λ ≈ 10²². λ = 1 occurs only for r ≤ 17, i.e. exactly the known solutions 4, 21 (start 2) and
6, 15, 715 (start 3), where the prediction is < 1. This is strong evidence of finiteness,
not a proof.

**Fourier attempt** (`phase6/riesz.py`): the idempotent set has an exact Riesz-product
Fourier transform, F(h) = ∏_q (1 + e(h·c_q/q)). Erdős–Turán would need
|F(h)| ≲ m^{−1/2} for h ≲ √m. Measured (2000 random h):
r = 97:  median ln|F| −28, max +11,  needed −42
r = 997: median ln|F| −16, max +38,  needed −478
The gap grows linearly in r. The set has only 2^k ≪ √m points, so no
equidistribution/discrepancy method can see a window of width √m. **This route is dead.**
A proof must use arithmetic structure specific to the window [2, √m], not equidistribution.

Barriers found so far:
1. abc gives nothing (P squarefree), unlike Brocard's problem.
2. The Pell / Richaud–Degert reformulation gives only class-number information.
3. Baker's theory is exponentially too weak; size arguments sit exactly at the trivial bound
   (gap of 1).
4. Fourier / Riesz-product equidistribution fails (set too sparse). Shown numerically above.
