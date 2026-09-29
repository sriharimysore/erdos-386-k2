#!/usr/bin/env python3
"""Experiment: for P = product of consecutive primes from s up to r, find the smallest n > 1
with 2P | n(n-1), by enumerating all 2^k idempotents mod P (Gray code), and report
lambda = n(n-1)/(2P). A solution to Erdos 386 (k=2) is exactly lambda = 1.
Also reports the heuristic prediction: the idempotents look like 2^k random points in [0,P),
so min n ~ P/2^k and lambda ~ P/(2*4^k)."""
import sys, math
def primes_upto(n):
    s = bytearray([1]) * (n + 1); s[0] = s[1] = 0
    for i in range(2, int(n**0.5) + 1):
        if s[i]: s[i*i::i] = bytes(len(range(i*i, n + 1, i)))
    return [i for i in range(n + 1) if s[i]]
PR = primes_upto(200)
def run(s, rmax):
    for r in [q for q in PR if s < q <= rmax]:
        block = [q for q in PR if s <= q <= r]
        k = len(block)
        if k > 24: break
        P = math.prod(block)
        m = 2 * P                       # need m | n(n-1); components: 2-power and odd primes
        mods = [4 if 2 in block else 2] + [q for q in block if q != 2]
        k = len(mods)
        basis = [(m // q) * pow(m // q, -1, q) % m for q in mods]
        e, best, gray = 0, None, 0
        for i in range(1, 1 << k):
            bit = (i & -i).bit_length() - 1
            gray ^= 1 << bit
            e = (e + basis[bit]) % m if gray >> bit & 1 else (e - basis[bit]) % m
            if e > 1 and (best is None or e < best): best = e
        assert best * (best - 1) % m == 0
        lam = best * (best - 1) // m
        pred = math.log10(P) - 2 * k * math.log10(2) - math.log10(2)
        print(f"s={s:2d} r={r:3d} k={k:2d}  log10 P={math.log10(P):6.2f}  min n={best:<22d}"
              f" lambda={lam}  log10 lambda={math.log10(lam):7.2f}  heuristic log10 ~ {pred:7.2f}")
        sys.stdout.flush()
for s in (2, 3):
    run(s, 200)
