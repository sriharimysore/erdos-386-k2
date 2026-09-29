#!/usr/bin/env python3
"""Empirical stress test of Lemma 1 in its general form, which does not need the primes
to be consecutive: if n >= 4, C(n,2) is odd and squarefree, and all its prime factors
lie in [p, r] with p >= 5, then p*r^h >= (2p-1)*p^h where h = floor(omega/2).
Checks every n <= LIMIT."""
import sys
LIMIT = int(sys.argv[1]) if len(sys.argv) > 1 else 3_000_000
spf = list(range(LIMIT + 1))
for i in range(2, int(LIMIT**0.5) + 1):
    if spf[i] == i:
        for j in range(i*i, LIMIT + 1, i):
            if spf[j] == j: spf[j] = i
def fac(m):
    out = []
    while m > 1:
        q = spf[m]; out.append(q); m //= q
    return out
checked = tight = 0
worst = None
for n in range(4, LIMIT + 1):
    A, O = (n // 2, n - 1) if n % 2 == 0 else ((n - 1) // 2, n)
    fs = sorted(fac(A) + fac(O))
    if fs[0] < 5 or len(set(fs)) != len(fs):
        continue
    p, r, h = fs[0], fs[-1], len(fs) // 2
    checked += 1
    assert p * r**h >= (2*p - 1) * p**h, (n, fs)
    ratio = (r / p) ** h / (2 - 1 / p)
    if worst is None or ratio < worst[0]: worst = (ratio, n, fs)
print(f"n <= {LIMIT}: {checked} odd squarefree C(n,2) with min prime >= 5 checked, 0 violations")
print(f"tightest case: ratio {worst[0]:.6f} at n = {worst[1]}, factors {worst[2]}")
