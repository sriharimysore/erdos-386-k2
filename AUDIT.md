# Pre-posting audit: Erdős #386 (k = 2)

Audit date: 2026-09-29. Adversarial review: the aim was to find problems, not to defend the
results.

## ⚠️ Errors and problems found (read first)

1. **ERROR (fixed): the balance lemma's statement did not match its sharpness example.**
   - The lemma was stated for blocks of *consecutive* primes.
   - The "sharp at n = 14" example, 91 = 7·13, is not consecutive (11 is skipped).
   - The proof never uses consecutiveness.
   - Fix: the lemma is now stated in its true generality: all prime factors of C(n,2) in
     [p, r] with p ≥ 5, and k = Ω counted with multiplicity. The block version is a corollary.
     The sharpness example is attributed to the general form only.
   - Changed in LEMMA.md, README.md and paper.tex. See Task 1.
2. **ERROR (fixed): off-by-one in the corollary.** LEMMA.md wrote "2·3·5⋯p₁₉₂₄" for the
   product of the first 1924 primes. In the file's own 0-indexed notation (p_0 = 2) that is
   1925 primes. Corrected to p_0⋯p_{1923}.
3. **MISLEADING WORDING (fixed): the paper's abstract.** It said "Unconditionally in n, any
   further solution must be … at least 137 consecutive primes". That result is *not*
   unconditional: it uses Dusart's theorem. The abstract now says "with no bound on n (but
   still using these results)".
4. **UNVERIFIED EXTERNAL DEPENDENCY (not fixed, can't be fixed without the paper):
   Dusart 2018 "Cor. 5.5"** is known only as quoted by Axler (arXiv:1703.08032). The
   original is paywalled. One automated search summary described Cor. 5.5 as a lower bound
   for π(x), not a short-interval result. That summary is unreliable, but it means the
   corollary number and/or statement **could be wrong**. Every result that depends on it
   (n ≤ 10⁹⁶¹⁴, k ≥ 1924) must be treated as **unverified**. See Task 3.
5. **SECOND-HAND LITERATURE CLAIMS.** The following were taken from search-engine summaries,
   not read at source:
   - the OEIS A280992 bound "n ≤ 5·10⁶" (Cloudflare blocked direct access),
   - "Nelson–Penney–Pomerance searched the first 3049 primes" (via a ScienceNews summary),
   - the Erdős–Graham "seems hopeless" quote.

   All three need human verification before being stated publicly as facts.
6. **INCOMPLETE RUN (not a claim).** `phase3/len6265.*` (Ramaré–Saouter, k ≤ 6265) never
   finished. It is untracked in git, and no claim is based on it. It must not be quoted.

No error was found in any computation. All cross-checks passed (Tasks 4 and 5 below).

---

## Task 1: the n = 14 inconsistency

**What the proof actually uses.** Reading the proof line by line, the hypotheses used are:
- (a) n ≥ 4, so that A = E/2 ≥ 2 and O ≥ 3 and both sides are non-empty;
- (b) every prime factor of C(n,2) is ≥ p with p ≥ 5, so A and O are odd, and it gives the
  lower bounds p^s ≤ A and p^t ≤ O;
- (c) every prime factor is ≤ r, which gives the upper bounds A ≤ r^s and O ≤ r^t;
- (d) k is the total number of prime factors, s + t = k.

Consecutiveness is never used, and neither is squarefreeness: counting with multiplicity,
p^s ≤ A ≤ r^s still holds.

**Precise statement (now in LEMMA.md, README.md, paper.tex).**
> *Lemma 1.* Let n ≥ 4, and suppose every prime factor of C(n,2) lies in [p, r] with p ≥ 5.
> Let k = Ω(C(n,2)) and h = ⌊k/2⌋. Then p·r^h ≥ (2p−1)·p^h.
>
> *Corollary 1′.* If C(n,2) = p_a⋯p_{a+k−1} with p_a ≥ 5, the inequality holds with
> p = p_a and r = p_{a+k−1}.

**Sharpness.**
- Equality forces the s = t case with s = 1, A = p, O = 2p − 1 (so k = 2).
- n = 14 (91 = 7·13) attains it, but only for the *general* form.
- For blocks, equality would need p and 2p−1 to be consecutive primes, which the
  Bertrand–Chebyshev theorem excludes for p ≥ 5. Nothing relies on this remark.

**Extra check.** `phase3/lemma_stress.py 3000000 --multiplicity` tests the general form,
including non-squarefree cases, on 499,999 values of n ≤ 3·10⁶: 0 violations. The tightest
case is n = 14 with ratio exactly 1.

---

## Task 2: line-by-line review of phase3/LEMMA.md

Legend: ✅ justified in full · 📚 relies on an external result · ⚠️ gap or issue.

| # | Step | Status | Notes |
|---|---|---|---|
| L0 | C(n,2) not prime for n ≥ 4: C(n,2) = (n/2)(n−1) or n·(n−1)/2, both factors ≥ 2 | ✅ | Also Lean-certified (`choose2_split`). Excludes blocks of length 1 in every search. |
| L1.1 | {n, n−1} = {E, O}; C(n,2) = A·O with A = E/2, gcd(A,O) = 1, \|2A − O\| = 1 | ✅ | gcd(n, n−1) = 1 ⇒ gcd(E/2, O) = 1. \|E − O\| = 1. This is where "ratio ≈ 2" comes from: O = 2A ± 1. |
| L1.2 | All prime factors ≥ 5 ⇒ A, O odd; write A, O as products of s, t primes in [p, r] | ✅ | Uses coprimality: every prime factor of C(n,2) lies in exactly one of A, O. |
| L1.3 | n ≥ 4 ⇒ A ≥ 2, O ≥ 3 ⇒ s, t ≥ 1 | ✅ | n = 4: E = 4, A = 2, O = 3. n odd ≥ 5: A = (n−1)/2 ≥ 2, O = n ≥ 5. (Under hypothesis (b), A ≥ 5 in fact.) |
| L1.4 | Case s = t: O/A = 2 ± 1/A ≥ 2 − 1/p and O/A ≤ (r/p)^s | ✅ | Uses A ≥ p (s ≥ 1) and O ≤ r^s, A ≥ p^s. Lean-certified (`balance_eq`). |
| L1.5 | Case s < t: p^{s+1} ≤ O ≤ 2A+1 ≤ 2r^s+1 ⇒ (r/p)^s ≥ p/2 − 1/(2p) ≥ 2 − 1/p | ✅ | Last step ⇔ p² − 4p + 1 ≥ 0, true for p ≥ 4. Lean-certified (`ineq_lt`, via `balance_core`). |
| L1.6 | Case s > t: 2p^{t+1} ≤ 2A ≤ O+1 ≤ r^t+1 ⇒ (r/p)^t ≥ 2p − p^{−t} ≥ 2 − 1/p | ✅ | Lean-certified (`ineq_gt`, via `balance_core`). |
| L1.7 | min(s,t) ≤ ⌊k/2⌋ and r ≥ p ⇒ (r/p)^{⌊k/2⌋} ≥ (r/p)^{min(s,t)} | ✅ | Lean-certified (`balance_mono`). |
| L1.8 | The p ≥ 5 restriction | ✅ | Needed so that A is odd with factors ≥ p, and for p² − 4p + 1 ≥ 0. **Blocks starting at 2 or 3 are never covered by Lemma 1.** Every search enumerates them fully (starts 2, 3, …, 37 are always enumerated), and the block-length theorem says explicitly that it makes no claim about long blocks starting at 2 or 3. |
| M | Monotonicity in k (fixed start) | ✅ | r and h are non-decreasing in k, and r/p ≥ 1. |
| L2 | 100·h·d < 68·p, p ≥ 41 ⇒ (r/p)^h < 2 − 1/p | ✅ | (1+x)^h ≤ e^{hx}. e^{0.68} ≈ 1.97388 < 2 − 1/41 ≈ 1.97561. Implemented as an exact integer test. Not formalized. |
| L3 | K = ⌈L/e⌉, e = ⌊log₂ p⌋ ⇒ blocks of length ≥ K exceed M | ✅ | p ≥ 2^e, product ≥ p^k ≥ 2^{eK} ≥ 2^L > M. |
| S1 | Search: starts p ≥ 41 excluded if L2 fails at length K−1; other starts enumerated for 2 ≤ k < K | ✅ | Soundness depends on L0–L3 and the code matching them. Code reviewed. Validated at N = 10⁹ with the pruning on and off (NOLEMMA=1): same answers. |
| S2 | QR filters: 1+8P square ⇒ (1+8P mod ℓ) not a non-residue | ✅ | Sound by definition. **The Jacobi implementation was adversarially tested** (`crosscheck/jacobi_test.c`): 0 mismatches against Euler's criterion in 2·10⁷ tests over all 88 filter primes used, and 0 true squares rejected. |
| S3 | Coverage: excluded + listed + 12 = π(X₀) = 486,570,088 | ✅ | `prime_count.py` was validated against the known values π(10⁶) = 78,498, π(10⁸) = 5,761,455 and π(10¹⁰) = 455,052,511. |
| T1 | Tail: from (G), p_{i+1} ≤ p_i(1+ε(p_i)) ≤ p_i(1+ε(p)) | 📚 | Needs (G) at every x = p_i ≥ X₀, and ε non-increasing (true for c/ln²x). |
| T2 | r ≤ p(1+ε)^{k−1} ⇒ (r/p)^h ≤ exp(ε(k−1)h) | ✅ | 1 + x ≤ e^x. |
| T3 | Corollary 1′ needs this ≥ 2 − 1/p ≥ 2 − 1/X₀; for n ≤ N, k ≤ ln M/ln p ≤ ln M/ln X₀ | ✅ | `tail_bounds.py` uses M < 10^{2D} (an upper bound on ln M, so conservative) and kmax(kmax−1)/2 ≥ (k−1)⌊k/2⌋. Strict inequality against ln 1.99 < ln(2 − 1/p). |
| T4 | Least admissible k for p ≥ X₀ is 137 / 1924 | 📚 | `length_bounds.py` (60-digit Decimal). Correct *given* the ε of each theorem. |
| B | Block-length theorem = T1–T4 for p ≥ X₀, plus search B for p < X₀, k ≤ K, plus L0 for k = 1 | ✅/📚 | Case split is complete: every start is either < X₀ (computed) or ≥ X₀ (tail); every k is 1, ≤ K, or > K. Starts 2 and 3 have all lengths ≤ K enumerated. |
| C | Corollary: C(n,2) ≥ product of the first 1924 primes | ✅ | Off-by-one in the index fixed (Error 2). |

**Edge cases checked.**
- *n even vs odd:* handled symmetrically by E/O (L1.1). n = 4 is the only case with
  A = 2, and it is excluded from Lemma 1 anyway (it contains the prime 2).
- *Block length 1:* Lemma 0.
- *Block length 2:* covered by Lemma 1 with h = 1 (k = 2 gives (r/p) ≥ 2 − 1/p). In the
  searches all length-2 blocks with start < X₀ are either enumerated or excluded. The
  unconditional phase-1 search enumerates every length-2 block with product ≤ M.
- *Empty block (p ≥ q in `Ico p q`):* product 1 = C(2,2), excluded by n ≥ 4.

---

## Task 3: external theorems

The argument uses exactly one kind of external input:
**(G) for all x ≥ x₀ there is a prime p with x < p ≤ x(1 + ε(x)), ε non-increasing.**

It is applied in LEMMA.md "Tail theorem" and "Block-length theorem", step 1, and in
`phase3/tail_bounds.py` and `phase3/length_bounds.py`. It is always applied at x = p_i, a
prime ≥ X₀ = 10,726,905,041. It is never applied below X₀; everything below X₀ is computed.

### (G1) Dusart 2010, Prop. 6.8 · **NEEDS HUMAN VERIFICATION**
- **As used:** x₀ = 396,738; ε(x) = 1/(25 ln²x) (natural log), with "≤" on the right
  endpoint. Applied only for x ≥ 1.0727·10¹⁰.
- **What I did see:** the text of arXiv:1002.0442v1, downloaded this session (pdftotext),
  states: "Proposition 6.8. For all x ⩾ 396 738, there exists a prime p such that
  x < p ⩽ x(1 + 1/(25 ln² x))." Its proof mentions validity "for ln x ⩾ 28" and "also valid
  from x ⩾ 3.8·10⁶" using a prime-gap table. Our range (x ≥ 1.07·10¹⁰) is inside what that
  argument claims to cover.
- **Checklist for you:**
  - [ ] The statement in the arXiv preprint matches the above (constant 25, ln², natural
        log, x₀ = 396738, "≤").
  - [ ] Whether the *published* version (Ramanujan J. 45 (2018), 227–251,
        doi:10.1007/s11139-016-9839-4) still contains this statement, and under what number.
        The preprint is not peer-reviewed as such.
  - [ ] Dusart's published correction (`correctif_RJ.pdf` on his homepage) fixes typos in
        Theorem 3.5. Check that it does not affect this proposition.
  - [ ] ε(x) is non-increasing for x ≥ 396738 (it is, for this formula; just confirm the
        formula).
- **Used for:** n ≤ 10⁶⁸² and k ≥ 137.

### (G2) Dusart 2018, "Cor. 5.5" · **NEEDS HUMAN VERIFICATION (HIGH PRIORITY)**
- **As used:** x₀ = 468,991,632; ε(x) = 1/(5000 ln²x). Applied only for x ≥ 1.0727·10¹⁰.
- **What I did see:** only the quotation in Axler, arXiv:1703.08032, §4: "Dusart [13,
  Corollary 5.5] … for every x ≥ 468 991 632 there exists a prime number p such that
  x < p ≤ x(1 + 1/(5 000 log² x))". I have **not** seen the original. An automated search
  summary described Corollary 5.5 differently (as a π(x) lower bound). That summary is
  unreliable, but it is a warning sign.
- **Checklist for you:**
  - [ ] Get the published paper (library, interlibrary loan, or email the author at the
        address on his homepage).
  - [ ] Find the short-interval statement, and confirm its number (5.5 or other), constant
        5000, log² (natural log), x₀ = 468,991,632, and the form of the interval.
  - [ ] Check the correction does not affect it.
- **Used for:** n ≤ 10⁹⁶¹⁴ and k ≥ 1924. **Do not post these until verified.**

### Not used in any claim
Ramaré–Saouter 2003 (via Axler) appears in `tail_bounds.py` / `length_bounds.py` output
only. The run that would have used it (k ≤ 6265) never finished.

### Range check
Both theorems are applied only at x ≥ X₀ = 1.0727·10¹⁰, which is above both x₀ values. No
application falls outside a claimed range. The computation covers every start below X₀,
including the whole interval [x₀, X₀).

---

## Task 4: independent cross-check (crosscheck/independent_search.py)

This was written from scratch, with no imports from phase1/ or phase3/. It uses pure Python,
exact integers and a plain sieve.
- **Block search, n ≤ 10⁸:** 8,332,385 blocks tested (including 1-prime blocks), 4,157,964
  primes sieved, **3.3 s**. Solutions **{4, 6, 15, 21, 715}: PASS**. Each was re-verified
  by trial-division factorization and a direct next-prime check.
- **Naive loop over every n ≤ 10⁶:** factor n and n−1 by trial division and check the
  factors of C(n,2) are distinct consecutive primes. **13.3 s**, solutions
  **{4, 6, 15, 21, 715}: PASS**.

Related adversarial checks:
- `crosscheck/jacobi_test.c`: filter correctness, **PASS**.
- `prime_count.py` against known π values, **PASS**.

---

## Task 5: Lean status

`lake build` succeeded (12 jobs). No `sorry`, `native_decide`, `admit`, `axiom`,
`implemented_by` or `extern` appears in any Lean source. `#print axioms`
(`crosscheck/Axioms.lean`):
```
'Erdos386.choose2_split' depends on axioms: [propext, Quot.sound]
'Erdos386.prod_bounds'   depends on axioms: [propext, Quot.sound]
'Erdos386.balance_eq'    depends on axioms: [propext, Quot.sound]
'Erdos386.ineq_lt'       depends on axioms: [propext, Quot.sound]
'Erdos386.ineq_gt'       depends on axioms: [propext, Quot.sound]
'Erdos386.balance_core'  depends on axioms: [propext, Quot.sound]
'Erdos386.balance_mono'  depends on axioms: [propext]
```
(`Erdos386.hello` in Basic.lean is the template's placeholder string, not a theorem.)

**What the Lean proves, and what it does not.**
- **Proves:**
  - Lemma 0 in the form "for n ≥ 4, n(n−1)/2 = a·b with a, b ≥ 2".
  - The purely arithmetic core of the balance lemma: if 2·∏LS and ∏LT differ by exactly 1,
    all list entries lie in [p, r] with p ≥ 5, and both lists are non-empty, then
    (2p−1)·p^h ≤ p·r^h for h = min(|LS|, |LT|).
  - That this inequality persists for larger exponents.
- **Does not prove:**
  - that primes, `Nat.nth Nat.Prime`, blocks, or C(n,2) = `n.choose 2` satisfy these
    hypotheses (the prime-splitting step);
  - Lemmas 2 and 3, the tail argument, or Dusart's theorems;
  - any of the computations.
- **The gap to the upstream statement:** there is no theorem in this repository whose
  statement mentions `Erdos386.erdos_386.variants.two`, `Nat.choose`, or `Nat.nth Nat.Prime`.
  None of the bounded results is Lean-certified.
- **Closing the gap needs Mathlib**, and still could not cover the billion-block
  computations or the analytic theorems.

---

## Addendum (2026-09-30): Mathlib formalization

Mathlib v4.34.1 installed. New file `Erdos 386/BalanceBlock.lean`, and `lake build` succeeds.
- `split_prod`: if A·O is a product of distinct primes f(i), i ∈ I, then A = ∏ of the f(i)
  dividing A and O = ∏ of the rest. This formalizes step L1.2 (the prime split).
- `choose2_ne_nth_prime`: Lemma 0 against `nth Nat.Prime`.
- `balance_block`: Corollary 1′ stated with the upstream objects
  (`n.choose 2 = ∏ i ∈ Finset.Ico a b, Nat.nth Nat.Prime i`, a ≥ 2).

`#print axioms`: `split_prod`, `choose2_ne_nth_prime` and `balance_block` depend on
`[propext, Classical.choice, Quot.sound]`. `Classical.choice` is new relative to the core
files. It is a standard Lean axiom, used throughout Mathlib; it is not `sorry` and not
`native_decide` (which would show `Lean.ofReduceBool`).

**What is now Lean-certified (updates Task 5):** the whole of Corollary 1′ (the balance
lemma for consecutive-prime blocks), stated in the upstream vocabulary.

**Still not formalized:**
- Lemmas 2 and 3,
- the tail argument and Dusart's theorems,
- all computations,
- any bounded-n result.

There is still no theorem about `erdos_386.variants.two` itself.

---

## Addendum (2026-09-30): OEIS A280992 verified at source; prior work found

The author supplied the text of the OEIS A280992 page.
- **Verified:** "No more terms up to the 5000000th triangular number." (CLAIMS #17: now yes.)
- **Prior work missed by the original write-up:** David A. Corneth (Oct 21 2017): "If a(8)
  exists, it's divisible by a prime p > prime(2000) = 17389." His PARI program `uptoprime`
  checks every block contained in the first n primes, via ratios of primorials. So every
  block using only primes ≤ 17389 was already excluded, reaching n ≈ 10³⁷⁷⁸.
- **Consequences:**
  - The claim "largest previous search n ≤ 5·10⁶" understated prior work. It has been
    corrected in the README, forum draft and paper.
  - Our n ≤ 10⁶⁸² (Dusart 2010) and k ≥ 137 results do **not** subsume Corneth's result.
  - n ≤ 10⁹⁶¹⁴ would, but it is unverified.
  - The small-start scan extends his bound for starts < 100 (end primes up to 4·10⁹, versus
    17389).
  - The balance lemma, the block-length theorem and the Lean formalization are unaffected.

---

## Addendum (2026-09-30): Dusart 2018 verified at source ✅

The author supplied the full text of Dusart, *Explicit estimates of some functions over
primes*, Ramanujan J. 45 (2018) 227–251, doi:10.1007/s11139-016-9839-4. Page 242 reads:

> **Corollary 5.5.** For all x ≥ 468 991 632, there exists a prime p such that
> x < p ≤ x(1 + (1/5000)/ln² x).

It is also stated in the introduction (p. 229): "for x ≥ 468 991 632, the interval
(x, x + x/(5000 ln² x)] contains at least one prime".

**G2 checklist:**
- [x] corollary number is 5.5,
- [x] constant 5000,
- [x] ln² x (the paper uses ln throughout),
- [x] x₀ = 468,991,632,
- [x] interval x < p ≤ x(1 + ε(x)).

The earlier search-summary claim (that Cor. 5.5 is a π(x) bound) was **wrong**: the π(x)
bounds are Corollaries 5.2 and 5.3.

**Correction note:** it fixes two misprints in the printed formula (3.3) (the published
paper's Theorem 3.1; the note calls it Theorem 3.5). The note says Table 1 was computed with
the correct formula, and later results rely on Table 1 and Theorem 4.2. So Cor. 5.5 is not
affected.

**Caveat found while reading:** the proof printed after Cor. 5.5 is written for the
1/ln³x form (Prop. 5.4, via maximal-gap tables up to 4·10¹⁸). The derivation of the
5000/ln²x form is not spelled out separately. It is a refereed published statement and we
rely on it as such; this is noted for completeness.

**Consequence for G1:** the published paper does **not** contain the 2010 preprint's
Prop. 6.8 (1/(25 ln²x)); that bound exists only in arXiv:1002.0442v1. This no longer
matters. For x ≥ X₀ ≥ 468,991,632, Cor. 5.5's interval is contained in Prop. 6.8's, so
every consequence of G1 (n ≤ 10⁶⁸², k ≥ 137) also follows from Cor. 5.5. **All conditional
results now rest on the single peer-reviewed Cor. 5.5.**

**Status change:** G2 moves from NEEDS HUMAN VERIFICATION to **verified at source**. Claims
11–15 are safe to post, with the caveat "conditional on Dusart 2018, Cor. 5.5". n ≤ 10⁹⁶¹⁴
subsumes Corneth's 2017 bound: every block of primes ≤ 17389 has product ≤ 17389#, i.e.
n ≲ 10³⁷⁴⁰.
