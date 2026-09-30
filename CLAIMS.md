# Claim inventory: README.md and paper/paper.tex

Status legend:
- **CC**: computation, cross-checked by an independent implementation.
- **C1**: computation, single implementation (with internal consistency checks only).
- **PR**: paper proof reviewed (AUDIT.md, Task 2).
- **EXT?**: depends on an external theorem that is unverified (AUDIT.md, Task 3).
- **LIT?**: literature fact taken second-hand, not read at source.
- **LEAN**: Lean-certified (kernel-checked; axioms propext and Quot.sound only).

| # | Claim | Depends on | Verification status | Safe to post? |
|---|---|---|---|---|
| 1 | Known solutions are n = 4, 6, 15, 21, 715 (with the factorizations shown) | direct arithmetic | CC (all implementations; each factored) | **yes** |
| 2 | No other solution with 4 ≤ n ≤ 10⁸ | independent_search.py; block_search.py; block_search.c | **CC** (3 independent implementations) | **yes** |
| 3 | No other solution with 4 ≤ n ≤ 10⁹ | block_search.py and block_search.c (same block counts); deep_search at N = 10⁹ with and without pruning | **CC** | **yes** |
| 4 | No other solution with 4 ≤ n ≤ 10¹² | block_search.c only (+ Lemma 0) | C1 at 10¹². Code cross-checked to 10⁹. Lemma 0 LEAN | **only with caveat** ("single C implementation at this range, cross-validated up to 10⁹") |
| 5 | Lemma 0: C(n,2) not prime for n ≥ 4 | elementary | PR + LEAN | **yes** |
| 6 | Balance lemma, general form (all prime factors in [p, r], p ≥ 5, k = Ω) | elementary | PR; LEAN (arithmetic core only); numerical: 499,999 cases, 0 violations | **yes** (say the Lean covers only the arithmetic core) |
| 7 | Balance lemma, block form (Corollary 1′) | claim 6 | PR | **yes** |
| 8 | General form is sharp (n = 14, 91 = 7·13), which is not a block | arithmetic | PR, checked numerically | **yes** |
| 9 | For blocks, equality is impossible for p ≥ 5 | Bertrand–Chebyshev theorem (classical) | PR (remark only, used nowhere) | yes, but unnecessary. Omit from the post. |
| 10 | Lemma 2 (integer test) and Lemma 3 (length cap) | elementary | PR | **yes** |
| 11 | No other solution with n ≤ 10⁶⁸² | claims 5–7, 10 + deep_search + Dusart 2010 Prop. 6.8 | PR + C1 (deep_search validated at 10⁹; coverage = π(X₀), with π validated) + **EXT?** (G1) | **only with caveat** ("conditional on Dusart 2010 Prop. 6.8, arXiv:1002.0442; published-version check pending") |
| 12 | No other solution with n ≤ 10⁹⁶¹⁴ | as 11, with Dusart 2018 "Cor. 5.5" | **EXT? (G2): statement seen only second-hand, corollary number in doubt** | **no**, not until G2 is verified |
| 13 | Any further solution has ≥ 137 consecutive primes, for all n | claims 5–7, 10 + deep_search k136 + Dusart 2010 | PR + C1 + EXT? (G1) | **only with caveat** (same as 11) |
| 14 | Any further solution has ≥ 1924 consecutive primes, for all n | as 13, with Dusart 2018 | EXT? (G2) | **no**, not until G2 is verified |
| 15 | Corollary: any further solution has C(n,2) ≥ product of the first 137 (or 1924) primes | claim 13 (or 14) | PR | same as 13 (or 14) |
| 16 | Blocks starting at a prime < 100 and ending at r ≤ 4·10⁹ give no new solution (incl. n(n−1) = r#, n(n−1) = 2·r#) | small_start_scan.c | C1 (validated at r ≤ 10⁶ against the known solutions; start-2 count = π(4·10⁹) − 1 matches the known π(4·10⁹); Jacobi filter tested) | **only with caveat** ("single implementation; filters sound, tested") |
| 17 | Previous largest published search: n ≤ 5·10⁶ (OEIS A280992) | OEIS entry | **LIT?** (search summary; OEIS page not read directly) | **only with caveat** ("the largest we found"). Verify on the OEIS page first. |
| 18 | Nelson–Penney–Pomerance searched the primorial case through the first 3049 primes | secondary (ScienceNews summary) | **LIT?** | **no** as a stated fact, unless checked in the paper. Can say "see [NPP]". |
| 19 | Erdős–Graham called a proof for k = 2 "hopeless" | search summary of erdosproblems.com | **LIT?** | **only with caveat**. Quote only after reading the source. |
| 20 | NPP citation details: J. Recreational Math. 7(2) (1974), 87–89 | web search | LIT? (consistent across sources) | yes, as a citation |
| 21 | Dusart 2018 citation: Ramanujan J. 45 (2018), 227–251, doi:10.1007/s11139-016-9839-4 | Crossref | verified (metadata only) | yes, as a citation |
| 22 | Idempotent experiment: min λ tracks P/(2·4^k) up to λ ≈ 10²² (starts 2, 3; r ≤ 97) | phase6/idempotents.py | C1 (exhaustive; reproduces the 5 solutions) | **yes, as heuristic evidence**, clearly labelled as not a proof |
| 23 | abc gives nothing because n(n−1) is squarefree up to the factor 2 | informal argument | informal, not reviewed by an expert | **only with caveat** ("our understanding") |
| 24 | Riesz-product / Fourier approach cannot work (set too sparse) | phase6/riesz.py (random sampling) | numerical heuristic only, not a proof of impossibility | **only with caveat** ("appears not to work, numerically") |
| 25 | Lean: the arithmetic core of the balance lemma and Lemma 0 are kernel-checked; no sorry or native_decide | lake build, #print axioms | **LEAN** | **yes**, and state the gap to `erdos_386.variants.two` |
| 26 | "The problem is still open. Nothing here solves it." | n/a | n/a | **yes** (must be included) |
