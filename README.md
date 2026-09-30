# Erdős Problem #386, case k = 2: computations and partial results

**Question** ([erdosproblems.com/386](https://www.erdosproblems.com/386); formal statement
`Erdos386.erdos_386.variants.two` in
[google-deepmind/formal-conjectures](https://github.com/google-deepmind/formal-conjectures)):
is C(n,2) = n(n−1)/2 a product of consecutive primes for infinitely many n?

Known solutions: n = 4, 6, 15, 21, 715
(6 = 2·3, 15 = 3·5, 105 = 3·5·7, 210 = 2·3·5·7, 255255 = 3·5·7·11·13·17).

**The problem is still open. Nothing here solves it.** This repository extends the verified
range and proves structural constraints on any further solution.

## Results

| # | Statement | Relies on | Where |
|---|---|---|---|
| 1 | Only n ∈ {4,6,15,21,715} for 4 ≤ n ≤ 10¹² | computation only | `phase1/` |
| 2 | Same for n ≤ 10⁹⁶¹⁴ | balance lemma + Dusart (2018), Cor. 5.5 (verified at source) | `phase3/` |
| 3 | For **every** n: any other solution is a product of ≥ 1924 consecutive primes | balance lemma + Dusart (2018), Cor. 5.5 | `phase3/LEMMA.md` |
| 4 | Blocks starting at any prime < 100 and ending at r ≤ 4·10⁹ give only the known solutions (includes n(n−1) = r#, the "714·715" primorial problem, through ~1.9·10⁸ primes) | computation only | `phase3/small_start_scan.c` |

**Prior work (OEIS [A280992](https://oeis.org/A280992)):**
- "No more terms up to the 5000000th triangular number", i.e. n ≤ 5·10⁶.
- D. A. Corneth (2017): any further term is divisible by a prime > prime(2000) = 17389. That
  is, every block of primes ≤ 17389 is already excluded, which reaches n < 10³⁷⁴² for blocks
  starting at small primes.

- erdosproblems.com forum, thread #386: StijnC (24 Aug 2025) observed that 2∏_I p_i and
  ∏_J p_j must differ by 1, so each fixed block length allows only finitely many solutions
  (via prime gaps). He also reported an independent search by D. Weisenberg: a new k = 2
  solution would need n > 10⁵⁰⁰. **Our balance lemma is an explicit, quantitative form of
  StijnC's observation**, and result 3 makes his "finitely many for fixed length" explicit.

Result 2 subsumes Corneth's bound: every block of primes ≤ 17389 gives n < 10³⁷⁴² < 10⁹⁶¹⁴.
Result 4 (end prime up to 4·10⁹ for starts < 100) goes further for small starting primes.

**External input:** results 2 and 3 use exactly one external theorem: Dusart, *Explicit
estimates of some functions over primes*, Ramanujan J. 45 (2018) 227–251, Cor. 5.5: for
x ≥ 468,991,632 there is a prime in (x, x(1 + 1/(5000 ln²x))]. This was verified in the
published text; see AUDIT.md. Nelson, Penney and Pomerance (1974) reportedly
searched the primorial case through the first 3049 primes (second-hand; not checked in the paper).

### The balance lemma
**General form (what the proof actually uses).** If n ≥ 4 and every prime factor of C(n,2)
lies in [p, r] with p ≥ 5, then (r/p)^⌊k/2⌋ ≥ 2 − 1/p, where k = Ω(C(n,2)) is the number of
prime factors counted with multiplicity. Why: n and n−1 differ by 1, so the prime factors
split into two groups whose products are in ratio almost exactly 2.

**Block form (what the searches use).** If C(n,2) = p_a ⋯ p_{a+k−1} is a block with
p_a ≥ 5, apply the general form with p = p_a and r = p_{a+k−1}.

**Sharpness.** The general form is sharp: n = 14, C(14,2) = 91 = 7·13, gives equality
13/7 = 2 − 1/7. That example is *not* a consecutive block (11 is skipped), so it says nothing
about sharpness of the block form. Full proofs are in [`phase3/LEMMA.md`](phase3/LEMMA.md).

### Lean 4 (kernel-checked)
Core Lean, no Mathlib:
- `Erdos 386/Balance.lean`: `balance_core` and `balance_mono`, the arithmetic core of the
  balance lemma.
- `Erdos 386/Lemma0.lean`: `choose2_split`: for n ≥ 4, C(n,2) = a·b with a, b ≥ 2.

With Mathlib (v4.34.1), stated with the **same objects as the upstream statement**
`erdos_386.variants.two` (`n.choose 2`, `∏ i ∈ Finset.Ico a b, Nat.nth Nat.Prime i`):
- `Erdos 386/BalanceBlock.lean`, `balance_block`: if n ≥ 4, a ≥ 2 and
  `n.choose 2 = ∏ i ∈ Finset.Ico a b, nth Nat.Prime i`, then
  `(2p − 1)·p^h ≤ p·r^h` with p = `nth Prime a`, r = `nth Prime (b−1)`, h = (b−a)/2.
  This includes a formal proof of the prime-splitting step (`split_prod`).
- `choose2_ne_nth_prime`: for n ≥ 4, `n.choose 2 ≠ nth Nat.Prime a` (blocks of length 1).

No `sorry`, no `native_decide`. Axioms:
- core files: `propext`, `Quot.sound`;
- Mathlib files: `propext`, `Classical.choice`, `Quot.sound` (Lean's three standard axioms).

Output is in `crosscheck/Axioms.lean`. Build with `lake build` (needs Mathlib, ~8 GB).

**Not yet formalized:**
- the prime-splitting step (needs Mathlib),
- the connection to the upstream `nth Nat.Prime` statement,
- the explicit prime-gap theorems,
- the computations.

The computations are ordinary C/Python programs, cross-validated against each other and
against independent prime counts, but not formally verified.

### Evidence and barriers (`phase6/`, NOTES.md)
Solutions correspond to "small idempotents" mod 2P. Their minimum tracks a random model up to
λ ≈ 10²², which is strong heuristic evidence that no further solutions exist. The notes also
record why several natural proof routes fail:
- abc (P is squarefree),
- Pell / Richaud–Degert,
- Baker's theory,
- Fourier / Riesz-product equidistribution.

## Audit
See [`AUDIT.md`](AUDIT.md) for the adversarial review, [`CLAIMS.md`](CLAIMS.md) for the trust
level of every claim, and `crosscheck/` for independent checks.

## Reproducing
```
cc -O3 -mcpu=native -o phase1/block_search phase1/block_search.c -lpthread
cc -O3 -mcpu=native -o phase3/deep_search phase3/deep_search.c -lpthread
cc -O3 -mcpu=native -o phase3/small_start_scan phase3/small_start_scan.c -lpthread
./phase1/block_search 1000000000000          # result 1   (~3 min, 8 cores)
./phase3/deep_search 63874 | python3 phase3/verify.py  # result 2 (L = bitlength of M for N = 10^9614, ~8 min)
./phase3/deep_search k1923 | python3 phase3/verify.py  # result 3
./phase3/small_start_scan 4000000000         # result 4   (~13 min)
```
Full log of methods, validation and open issues: [`NOTES.md`](NOTES.md).

## Credits
By Srihari Mysore. All claims above are meant to be independently checkable, and
corrections are welcome.
