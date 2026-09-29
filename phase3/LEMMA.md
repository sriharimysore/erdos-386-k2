# The balance lemma and the deep search: proofs

Notation: p_0 = 2, p_1 = 3, … (Mathlib's `nth Nat.Prime`). A block is
B(a,k) = p_a · p_{a+1} ⋯ p_{a+k−1}: k consecutive primes, first p = p_a, last r = p_{a+k−1}.
M = N(N−1)/2.

## Lemma 0 (length 1)
For n ≥ 4, C(n,2) is not prime.
*Proof.* C(n,2) = (n/2)(n−1) if n is even, n·((n−1)/2) if n is odd. Both factors are ≥ 2
when n ≥ 4. ∎

## Lemma 1 (balance)
Let n ≥ 4 and suppose C(n,2) = B(a,k) with p = p_a ≥ 5. Let r = p_{a+k−1} and h = ⌊k/2⌋. Then

    p · r^h ≥ (2p − 1) · p^h,    i.e.  (r/p)^h ≥ 2 − 1/p.

*Proof.* Write {n, n−1} = {E, O} with E even and O odd. Then C(n,2) = A·O with A = E/2, and
|2A − O| = |E − O| = 1. All block primes are ≥ 5, so C(n,2) is odd and squarefree. The
factors A and O are coprime, so the block's primes split as S ⊔ T with A = ∏S and O = ∏T.
Here A ≥ 2 and O ≥ 3 (since n ≥ 4), so S and T are non-empty. Let s = |S| and t = |T|,
so s + t = k. Every prime in the block lies in [p, r].

- **s = t (= h).** O/A = 2 ± 1/A, so O/A ≥ 2 − 1/A ≥ 2 − 1/p. Also O/A ≤ r^s/p^s.
  Hence (r/p)^h ≥ 2 − 1/p.
- **s < t.** Then t ≥ s+1, so p^{s+1} ≤ p^t ≤ O ≤ 2A + 1 ≤ 2r^s + 1. Therefore
  (r/p)^s ≥ p/2 − 1/(2p^s) ≥ p/2 − 1/(2p), and this is ≥ 2 − 1/p because p² − 4p + 1 ≥ 0 for p ≥ 4.
- **s > t.** Then s ≥ t+1, so 2p^{t+1} ≤ 2A ≤ O + 1 ≤ r^t + 1. Therefore
  (r/p)^t ≥ 2p − p^{−t} ≥ 2 − 1/p.

In each case the exponent is min(s,t) ≤ ⌊k/2⌋ = h. Since r/p ≥ 1, (r/p)^h ≥ (r/p)^{min(s,t)}. ∎

**Monotonicity.** Increasing k (with a fixed) does not decrease r or h, so the condition of
Lemma 1 is monotone in k. If it fails at length k₀, it fails at every length k ≤ k₀.

## Lemma 2 (a cheap sufficient test for failure)
Let p ≥ 41, d = r − p, h ≥ 0. If 100·h·d < 68·p, then (r/p)^h < 2 − 1/p.
*Proof.* (1 + d/p)^h ≤ e^{hd/p} < e^{0.68} < 1.9739 < 1.9756 < 2 − 1/41 ≤ 2 − 1/p. ∎
(This is an exact integer test. No floating point is involved.)

## Lemma 3 (length cap)
Let e = ⌊log₂ p⌋ (= bitlength(p) − 1) and L = bitlength(M), so that M < 2^L. Set
K = ⌈L/e⌉. Every block starting at p with k ≥ K primes has product ≥ p^k ≥ 2^{eK} ≥ 2^L > M.

## Search theorem (computational part)
For every start p_a with 41 ≤ p_a < X₀, let k₀ = K − 1. If Lemma 2 shows Lemma 1's condition
fails at length k₀, then by monotonicity it fails for all k ≤ k₀. Blocks with k ≥ K are too
big. So no block starting at p_a is a C(n,2) with n ≤ N, and the start is **excluded**.
Every other start (p < 41, or not excluded) is **enumerated**: every length 2 ≤ k < K is
tested.

A block is tested with exact modular filters: if 1 + 8P is a square, then (1 + 8P mod ℓ) is
a square mod ℓ for each prime ℓ. Blocks passing every filter are re-checked in Python with
exact big-integer isqrt.

## Tail theorem (analytic part): starts p ≥ X₀
Assume an explicit short-interval theorem:
(G) for every x ≥ X₀ there is a prime in (x, x(1+ε(x))], with ε non-increasing.
Take a block with start p ≥ X₀ and product ≤ M. Each consecutive gap satisfies
p_{i+1} ≤ p_i(1+ε(p_i)) ≤ p_i(1+ε(p)), so r ≤ p(1+ε)^{k−1}, and

    (r/p)^h ≤ exp(ε·(k−1)·k/2).

Lemma 1 needs this to be ≥ 2 − 1/p > 1.99, i.e. ε·k(k−1)/2 ≥ ln 1.99 > 0.688, i.e.
k(k−1) > 1.376/ε. But product ≤ M forces p^k ≤ M, i.e. k ≤ ln M / ln p ≤ ln M / ln X₀.
So if (ln M/ln X₀)·(ln M/ln X₀ − 1) ≤ 1.376/ε(X₀), no start ≥ X₀ can give a solution. (ε is
non-increasing, so the check at X₀ covers every p ≥ X₀ in each case below. With a constant ε
the worst case is p = X₀, where k is largest.)

Instances of (G):
| source | X₀ | ε(x) | max ln M allowed | ≈ max N |
|---|---|---|---|---|
| Dusart 2010, Prop. 6.8 (arXiv:1002.0442) | 396,738 | 1/(25 ln²x) | see below | |
| Dusart 2018, Cor. 5.5 (Ramanujan J.) | 468,991,632 | 1/(5000 ln²x) | | |
| Ramaré–Saouter 2003, Thm 3 (J. Number Theory 98) | 10,726,905,041 | 1/28,313,999 | | |

Each theorem's X₀ is ≤ 1.0727·10¹⁰. The computational part checks every start up to that
bound, so for every row the tail starts at X₀* = 10,726,905,041. For the ln²x rows, the
bound becomes 1.376·C·ln²x evaluated at x = X₀* (ε non-increasing).
The actual limits are computed exactly in `tail_bounds.py`.

## Block-length theorem (no bound on n)

**Theorem.** Let n ≥ 4, and suppose C(n,2) = p_a ⋯ p_{a+k−1} is a product of k consecutive
primes. If n ∉ {4, 6, 15, 21, 715}, then
- k ≥ 137, assuming Dusart 2010 Prop. 6.8, and
- k ≥ 1924, assuming Dusart 2018 Cor. 5.5.

(No claim is made about where the block starts. Long blocks starting at 2 or 3 are not
ruled out.)

*Proof.* Let K be 136 (respectively 1923).
1. **Starts p_a ≥ X₀ = 10,726,905,041.** By the gap theorem, each step satisfies
   p_{i+1} ≤ p_i(1 + ε(X₀)), so (r/p)^⌊k/2⌋ ≤ exp(ε(X₀)(k−1)⌊k/2⌋). Lemma 1 needs this to be
   ≥ 2 − 1/p ≥ 2 − 1/X₀. `length_bounds.py` computes the least k for which that is possible:
   137 (Dusart 2010) and 1924 (Dusart 2018).
2. **Starts p_a < X₀ and k ≤ K.** Checked by computer: `deep_search kK`.
   - A start p ≥ 41 is excluded when Lemma 2 shows Lemma 1 fails at length K. By
     monotonicity it then fails at every k ≤ K.
   - Every other start (including 2, 3, …, 37) has all lengths 2 ≤ k ≤ K tested with exact
     modular filters. Survivors are re-checked with exact isqrt.
   - The only blocks found are the five known solutions.
   - Coverage: excluded + listed + 12 = π(X₀) = 486,570,088, computed independently.
   - Runs: K = 136 took 20 s; K = 1923 took 138 s, with 1,859,956,721 blocks tested and
     only the 5 known solutions surviving.
3. **k = 1.** Excluded by Lemma 0.

For start 2 or 3 the block is 2·3·5⋯ or 3·5⋯, and step 2 covered every length k ≤ K. So any
other solution with k ≤ K would have been found. ∎

**Corollary.** Any further solution has C(n,2) ≥ 2·3·5⋯p₁₉₂₄ (the smallest product of
1924 consecutive primes), so n is astronomically large. Combined with the deep search, n > 10⁹⁶¹⁴ under
Dusart 2018.

### Lean status
`Erdos 386/Balance.lean` (Lean 4 core, no Mathlib) machine-checks the arithmetic core of
Lemma 1:
- `balance_core`: if 2·∏LS and ∏LT differ by 1, with all factors in [p, r] and p ≥ 5, then
  (2p−1)·p^h ≤ p·r^h for h = min(|LS|, |LT|).
- `balance_mono`: extends this to any exponent h′ ≥ h.

Axioms used: propext and Quot.sound only. No sorry, no native_decide, fully kernel-checked.

Not yet formalized:
- the number-theoretic split of the block's primes into S and T (needs Mathlib),
- Lemmas 0, 2, 3,
- the gap theorems,
- the computation.
