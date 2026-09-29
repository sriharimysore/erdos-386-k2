#!/usr/bin/env python3
"""Independent pi(x) via the Lucy_Hedgehog / Legendre-style dynamic program, O(x^{3/4}).
Used to confirm that Phase A of deep_search visited every prime <= X0."""
import sys
from math import isqrt
def pi(n):
    r = isqrt(n)
    V = [n // i for i in range(1, r + 1)]
    V += list(range(V[-1] - 1, 0, -1))
    S = {v: v - 1 for v in V}
    for p in range(2, r + 1):
        if S[p] > S[p - 1]:
            sp, p2 = S[p - 1], p * p
            for v in V:
                if v < p2: break
                S[v] -= S[v // p] - sp
    return S[n]
x = int(sys.argv[1]); print(f"pi({x}) = {pi(x)}")
