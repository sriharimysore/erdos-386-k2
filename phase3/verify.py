#!/usr/bin/env python3
"""Exact re-check of deep_search survivors. Reads deep_search output on stdin.
For each "SURVIVOR p k": compute P = product of k consecutive primes from p exactly and
test whether 1 + 8P is a perfect square (math.isqrt). Prints the n values found."""
import sys
from math import isqrt
lines = sys.stdin.read().splitlines()
surv = [tuple(map(int, l.split()[1:])) for l in lines if l.startswith("SURVIVOR")]
need = max((p for p, _ in surv), default=2) + 2 * 10**7
sieve = bytearray([1]) * (need + 1); sieve[0] = sieve[1] = 0
for i in range(2, isqrt(need) + 1):
    if sieve[i]: sieve[i*i::i] = bytes(len(range(i*i, need + 1, i)))
primes = [i for i in range(need + 1) if sieve[i]]
index = {q: i for i, q in enumerate(primes)}
sols, false_pos = [], 0
for p, k in surv:
    a = index[p]; assert a + k <= len(primes)
    P = 1
    for q in primes[a:a + k]: P *= q
    D = 1 + 8 * P; s = isqrt(D)
    if s * s == D: sols.append(((1 + s) // 2, p, k))
    else: false_pos += 1
for n, p, k in sorted(sols):
    print(f"SOLUTION n = {n}  block = {k} primes from {p}")
print(f"{len(surv)} survivors, {len(sols)} genuine, {false_pos} filter false positives")
