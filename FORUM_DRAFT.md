<!-- DRAFT — do not post until the checklists in AUDIT.md (Task 3, and the OEIS check in
CLAIMS.md #17) are done. Only claims marked "yes" or "only with caveat" in CLAIMS.md are
used, with their caveats. -->

**k = 2: a bounded verification and a structural constraint. Not a solution.**

For k = 2 (is n(n−1)/2 a product of consecutive primes infinitely often?) I have some
computations and one elementary lemma. I'd welcome corrections and pointers to prior work.
Code, logs and proofs: [REPO LINK]

**1. Search (computation only).**
- No solutions besides n = 4, 6, 15, 21, 715 for n ≤ 10⁹. Three independent programs agree.
- The same holds for n ≤ 10¹², from a single C program that is cross-validated against the
  others up to 10⁹.
- OEIS A280992 records no further terms for n ≤ 5·10⁶. D. A. Corneth's comment there shows any
  further term has a prime factor > prime(2000) = 17389. That covers every block of primes
  ≤ 17389, which is far beyond 10¹² for blocks starting at small primes.

**2. A balance lemma (elementary; block form machine-checked in Lean 4 + Mathlib).**
- *Statement.* If n ≥ 4 and every prime factor of C(n,2) lies in [p, r] with p ≥ 5, then
  (r/p)^⌊k/2⌋ ≥ 2 − 1/p, where k = Ω(C(n,2)).
- *Idea.* Write C(n,2) = A·O with A = E/2, where E and O are the even and odd members of
  {n, n−1}. Then O = 2A ± 1, so the prime factors split into two groups whose products are
  in ratio ≈ 2.
- *Sharpness.* The bound is sharp: n = 14 gives 91 = 7·13 and 13/7 = 2 − 1/7. That example
  is not a consecutive block.
- *Consequence for the problem.* A block of consecutive primes starting at a large prime
  must be long, so its product is huge.

**3. Consequences, conditional on one explicit prime-gap theorem.** The theorem is Dusart,
Ramanujan J. 45 (2018), Cor. 5.5: for x ≥ 468,991,632 there is a prime in
(x, x(1 + 1/(5000 ln²x))]. Using it for starting primes ≥ 1.07·10¹⁰, and computing
everything below that:
- no solutions besides the five for **n ≤ 10⁹⁶¹⁴**. This contains the region covered by
  Corneth's bound (every block of primes ≤ 17389 has n < 10³⁷⁴²);
- for **all** n, any further solution is a product of **at least 1924 consecutive primes**.

**4. Small starting primes (computation only, single implementation).**
- Blocks starting at any prime < 100 and ending at a prime ≤ 4·10⁹ give no new solution.
- In particular, n(n−1) = r# (the "714·715" question) has no new solution for r ≤ 4·10⁹.
- For starting primes below 100 this goes beyond Corneth's r ≤ 17389 (and beyond n ≤ 10⁹⁶¹⁴).

**5. Heuristics.**
- *Random model.* The n with 2P | n(n−1) are 2^k residues mod 2P. For blocks starting at
  2 or 3, the least one tracks the random-model prediction λ ≈ P/(2·4^k) up to λ ≈ 10²² (r ≤ 97).
- *abc.* As far as I can tell, abc gives nothing here, since n(n−1) is squarefree up to
  the factor 2.

**Trust level.**
- The computations are ordinary C/Python programs, not formally verified.
- In Lean 4 + Mathlib (no sorry, no native_decide; standard axioms only), the balance lemma
  for blocks is proved with the upstream objects: `n.choose 2 = ∏ i ∈ Finset.Ico a b,
  Nat.nth Nat.Prime i`, a ≥ 2. Lemma 0 is also proved.
- The bounded results and Dusart's theorem are not formalized.

**Disclosure.** This work was done with substantial assistance from Claude (Anthropic), an
AI model.

Is the balance lemma already known, and has anyone searched further than this? Pointers to
either would be very welcome.
