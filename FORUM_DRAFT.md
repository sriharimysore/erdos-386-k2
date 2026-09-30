**k = 2: making StijnC's argument explicit, and a bounded verification. Not a solution.**

Building on @StijnC's observation (24 Aug 2025) that 2∏_I p_i and ∏_J p_j must differ by
exactly 1, so that each fixed block length allows only finitely many solutions, I tried to
make this fully explicit and push it as far as possible. Code, logs and proofs:
https://github.com/sriharimysore/erdos-386-k2

**1. A quantitative form of the balance argument.**
- *Statement.* If n ≥ 4 and every prime factor of C(n,2) lies in [p, r] with p ≥ 5, then
  (r/p)^⌊k/2⌋ ≥ 2 − 1/p, where k = Ω(C(n,2)).
- *Proof idea.* Write C(n,2) = A·O with O = 2A ± 1 and split the primes between A and O.
  There are three cases: |S| = |T|, |S| < |T|, |S| > |T|.
- *Sharpness.* n = 14, 91 = 7·13 gives equality. That example is not a consecutive block.
- *Lean.* The block form is machine-checked in Lean 4 + Mathlib, stated with the upstream
  objects `n.choose 2 = ∏ i ∈ Finset.Ico a b, Nat.nth Nat.Prime i` (a ≥ 2). No sorry, no
  native_decide.

**2. Explicit consequences.** These are conditional on one prime-gap theorem: Dusart,
Ramanujan J. 45 (2018), Cor. 5.5, which says that for x ≥ 468,991,632 there is a prime in
(x, x(1 + 1/(5000 ln²x))]. I use it for starting primes ≥ 1.07·10¹⁰ and a computer search
below that.
- **For all n**, any further solution is a product of **at least 1924 consecutive primes**.
  This makes StijnC's "finitely many for each fixed length" explicit. In fact there are
  none with ≤ 1923 primes.
- No solutions besides 4, 6, 15, 21, 715 for **n ≤ 10⁹⁶¹⁴**. This extends Desmond's search
  (n > 10⁵⁰⁰, per StijnC's comment), and it also covers Corneth's OEIS A280992 bound (any
  further term has a prime factor > 17389).

**3. Computation only (no prime-gap input; ordinary C/Python, not formally verified).**
- No further solution for n ≤ 10¹². Three independent programs agree up to 10⁹, and one C
  program was run to 10¹².
- Blocks starting at any prime < 100 and ending at a prime ≤ 4·10⁹ give nothing new. In
  particular, n(n−1) = r# has no new solution for r ≤ 4·10⁹.

**4. A remark on why the long-block case looks hard.** The n with 2P | n(n−1) are 2^k
residues mod 2P. For blocks starting at 2 or 3, the least one tracks the random-model
prediction λ = n(n−1)/(2P) ≈ P/(2·4^k), up to λ ≈ 10²² (r ≤ 97). As far as I can tell, abc
gives nothing here, since n(n−1) is squarefree up to the factor 2.

**Disclosure.** This was done with substantial assistance from Claude (Anthropic), an AI
model. Corrections are very welcome, especially on the explicit constants in part 2.
