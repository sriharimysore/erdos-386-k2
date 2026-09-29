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
| 2 | Same for n ≤ 10⁶⁸² | balance lemma + Dusart (2010), Prop. 6.8 | `phase3/` |
| 3 | Same for n ≤ 10⁹⁶¹⁴ | balance lemma + Dusart (2018), Cor. 5.5 | `phase3/` |
| 4 | For **every** n: any other solution uses ≥ 137 consecutive primes (≥ 1924 under Dusart 2018) | balance lemma + the same theorems | `phase3/LEMMA.md` |
| 5 | Blocks starting at any prime < 100 and ending at r ≤ 4·10⁹ give only the known solutions (includes n(n−1) = r#, the "714·715" primorial problem, through ~1.9·10⁸ primes) | computation only | `phase3/small_start_scan.c` |

For comparison, the largest previously published search we found is n ≤ 5·10⁶
(OEIS [A280992](https://oeis.org/A280992)). Nelson, Penney and Pomerance (1974) searched
the primorial case through the first 3049 primes.

### The balance lemma
If C(n,2) = p_a ⋯ p_{a+k−1} with smallest prime p ≥ 5 and largest prime r, then
(r/p)^⌊k/2⌋ ≥ 2 − 1/p. Why: n and n−1 differ by 1, so the block's primes split into two
groups whose products are in ratio almost exactly 2. The bound is sharp (equality at
n = 14, where 91 = 7·13). Full proofs are in [`phase3/LEMMA.md`](phase3/LEMMA.md).

### Lean 4 (kernel-checked, core Lean only, no Mathlib)
- `Erdos 386/Balance.lean`: `balance_core` and `balance_mono`, the arithmetic core of the
  balance lemma.
- `Erdos 386/Lemma0.lean`: `choose2_split`: for n ≥ 4, C(n,2) = a·b with a, b ≥ 2.

Axioms used: `propext`, `Quot.sound`. No `sorry`, no `native_decide`. Build with `lake build`.

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

## Reproducing
```
cc -O3 -mcpu=native -o phase1/block_search phase1/block_search.c -lpthread
cc -O3 -mcpu=native -o phase3/deep_search phase3/deep_search.c -lpthread
cc -O3 -mcpu=native -o phase3/small_start_scan phase3/small_start_scan.c -lpthread
./phase1/block_search 1000000000000          # result 1   (~3 min, 8 cores)
./phase3/deep_search 4531 | python3 phase3/verify.py   # result 2 (L = bitlength of M for N = 10^682)
./phase3/deep_search k1923 | python3 phase3/verify.py  # result 4
./phase3/small_start_scan 4000000000         # result 5   (~13 min)
```
Full log of methods, validation and open issues: [`NOTES.md`](NOTES.md).

## Credits and AI disclosure
Project directed by Srihari Mysore. The code, proofs and Lean formalization were developed
with substantial assistance from Claude (Anthropic), an AI model. All claims above are
meant to be independently checkable, and corrections are welcome.
