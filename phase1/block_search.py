#!/usr/bin/env python3
"""Erdős #386, k = 2: find every n <= N with C(n,2) a product of consecutive primes.

Reference implementation (pure Python, exact integer arithmetic only).

Method: enumerate blocks of consecutive primes p_a * p_{a+1} * ... * p_{b-1}
whose product P is <= M = N(N-1)/2, and test whether P = n(n-1)/2 for some n,
i.e. whether 1 + 8P is a perfect square s^2 (then n = (1+s)/2).

Usage: python3 block_search.py N
"""
import sys
import time
from array import array
from itertools import compress
from math import isqrt


def primes_upto(L):
    """All primes <= L, as a compact array('Q'). Odd-only Eratosthenes sieve."""
    if L < 2:
        return array('Q')
    # sieve[i] represents the odd number 2i+1
    size = (L + 1) // 2
    sieve = bytearray([1]) * size
    sieve[0] = 0  # 1 is not prime
    for i in range(1, (isqrt(L) - 1) // 2 + 1):
        if sieve[i]:
            p = 2 * i + 1
            start = p * p // 2
            sieve[start::p] = bytes(len(range(start, size, p)))
    ps = array('Q', [2])
    ps.extend(compress(range(1, 2 * size, 2), sieve))
    return ps


def triangular_root(P):
    """Return n if P = n(n-1)/2 for an integer n >= 1, else None. Exact."""
    D = 1 + 8 * P          # always odd
    s = isqrt(D)
    if s * s != D:
        return None
    # s is odd because D is odd, so (1+s)/2 is an integer
    return (1 + s) // 2


def search(N):
    """Return (solutions, stats). solutions: list of (n, a, b) with
    C(n,2) = prod_{i in [a,b)} nth_prime(i), n <= N, 0-indexed like Mathlib."""
    M = N * (N - 1) // 2
    # Any block of length >= 2 has its second-to-last prime <= isqrt(M), so its
    # last prime is at most the first prime > isqrt(M). Prime gaps below 1e12
    # are < 1000, so sieving to isqrt(M) + 2000 suffices; we assert it anyway.
    L = isqrt(M) + 2000
    t0 = time.perf_counter()
    ps = primes_upto(L)
    t_sieve = time.perf_counter() - t0
    assert ps[-1] > isqrt(M), "sieve too short to contain successor of sqrt(M)"

    sols = []
    blocks = 0
    t1 = time.perf_counter()
    np_ = len(ps)
    for a in range(np_):
        P = ps[a]
        # Length-1 blocks: handled here only for primes in the sieve. (Lemma:
        # C(n,2) is never prime for n >= 4, so larger single primes are moot.)
        if P <= M:
            blocks += 1
            n = triangular_root(P)
            if n is not None and n <= N:
                sols.append((n, a, a + 1))
        if a + 1 >= np_:
            break
        if P * ps[a + 1] > M:
            break  # no block of length >= 2 starts at p_a or later
        b = a + 1
        while True:
            assert b < np_, "block ran past sieve end"
            P *= ps[b]
            b += 1
            if P > M:
                break
            blocks += 1
            n = triangular_root(P)
            if n is not None:
                sols.append((n, a, b))
    t_search = time.perf_counter() - t1
    return sols, dict(N=N, M=M, sieve_limit=L, primes=np_, blocks=blocks,
                      t_sieve=t_sieve, t_search=t_search)


def main():
    N = int(sys.argv[1])
    sols, st = search(N)
    print(f"N = {N}  (M = N(N-1)/2 = {st['M']})")
    print(f"sieve limit {st['sieve_limit']}, primes {st['primes']}, "
          f"blocks tested {st['blocks']}")
    print(f"time: sieve {st['t_sieve']:.2f}s, block search {st['t_search']:.2f}s")
    for n, a, b in sorted(sols):
        ps = primes_upto(max(100, 2 * n))[a:b]
        print(f"  n = {n:>6}  C(n,2) = {n*(n-1)//2}  = prod nth_prime [{a},{b}) = "
              + "*".join(map(str, ps)))


if __name__ == "__main__":
    main()
