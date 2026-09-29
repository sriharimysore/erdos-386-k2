#!/usr/bin/env python3
"""Can Fourier analysis (Erdos-Turan with the exact Riesz product F(h)) prove that no
idempotent lies in [2, sqrt(m)]? Erdos-Turan needs sum_{h<=H} |F(h)|/h << 1 with H ~ sqrt(m).
We measure the typical size of |F(h)| = 2^k prod |cos(pi h c_q / q)| over h."""
import math, random
def primes_upto(n):
    s = bytearray([1]) * (n + 1); s[0] = s[1] = 0
    for i in range(2, int(n**0.5) + 1):
        if s[i]: s[i*i::i] = bytes(len(range(i*i, n + 1, i)))
    return [i for i in range(n + 1) if s[i]]
random.seed(0)
for r in (97, 199, 499, 997):
    block = [q for q in primes_upto(r) if q >= 3]
    m = 2 * math.prod(block); mods = [2] + block; k = len(mods)
    c = [pow(m // q, -1, q) for q in mods]
    logs = []
    for _ in range(2000):
        h = random.randrange(1, 10**12)
        s = sum(math.log(abs(math.cos(math.pi * (h * cq % q) / q)) + 1e-300) for cq, q in zip(c, mods))
        logs.append(k * math.log(2) + s)
    logs.sort()
    need = -0.5 * math.log(m)          # Erdos-Turan needs |F(h)| <~ H^{-1} ~ m^{-1/2}
    print(f"r={r:4d} k={k:3d}  ln m={math.log(m):8.1f}  median ln|F(h)|={logs[1000]:7.1f}"
          f"  max={logs[-1]:7.1f}  needed <~ {need:8.1f}")
