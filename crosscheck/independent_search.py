#!/usr/bin/env python3
"""Independent cross-check for Erdős #386 (k = 2), written from scratch.
Imports nothing from phase1/ or phase3/.

Part 1 (block search, n <= 10^8):
  For every run of consecutive primes q_i * q_{i+1} * ... * q_j with product B <= n_max(n_max-1)/2,
  B = C(n,2) for some n iff 8B + 1 is a perfect square (then n = (1 + sqrt(8B+1)) / 2).
  Blocks of one prime are included here too. None qualifies for n >= 4, but including them
  avoids relying on that lemma.
Part 2 (naive, n <= 10^6):
  For each n, factor C(n,2) by trial division and check directly that its prime factors are
  distinct and consecutive (no prime strictly between neighbours, checked by trial division).
Every solution from either part is re-verified by factorization.
"""
import math
import sys
import time


def sieve(limit):
    """Plain sieve of Eratosthenes. Returns the list of primes <= limit."""
    is_p = bytearray([1]) * (limit + 1)
    is_p[0] = 0
    if limit >= 1:
        is_p[1] = 0
    i = 2
    while i * i <= limit:
        if is_p[i]:
            is_p[i * i :: i] = bytes(len(range(i * i, limit + 1, i)))
        i += 1
    return [x for x in range(limit + 1) if is_p[x]]


def is_prime_td(m):
    """Trial division primality test (deliberately naive)."""
    if m < 2:
        return False
    d = 2
    while d * d <= m:
        if m % d == 0:
            return False
        d += 1
    return True


def factor_td(m):
    """Prime factorization by trial division, with multiplicity."""
    out, d = [], 2
    while d * d <= m:
        while m % d == 0:
            out.append(d)
            m //= d
        d += 1
    if m > 1:
        out.append(m)
    return out


def next_prime_td(a):
    """Smallest prime > a, by trial division."""
    c = a + 1
    while not is_prime_td(c):
        c += 1
    return c


def is_consecutive_prime_product(m, factors=None):
    """Direct definition: m > 1 is a product of distinct primes, and each factor's successor
    among the factors is its next prime (nothing skipped)."""
    fs = sorted(factors if factors is not None else factor_td(m))
    if len(set(fs)) != len(fs):
        return False
    return all(next_prime_td(a) == b for a, b in zip(fs, fs[1:]))


def block_search(n_max):
    bound = n_max * (n_max - 1) // 2
    # Any block of >= 2 primes has smallest prime q with q*q < bound. Its last prime is at
    # most the first prime above sqrt(bound). One-prime blocks up to the sieve limit are also
    # checked (none can work for n >= 4).
    limit = math.isqrt(bound) + 10_000
    primes = sieve(limit)
    assert primes[-1] > math.isqrt(bound)
    found, tested = [], 0
    for i in range(len(primes) - 1):
        prod = 1
        j = i
        while j < len(primes):
            prod *= primes[j]
            if prod > bound:
                break
            tested += 1
            d = 8 * prod + 1
            s = math.isqrt(d)
            if s * s == d:
                n = (1 + s) // 2
                if n >= 4:
                    found.append((n, primes[i], primes[j]))
            j += 1
        else:
            raise RuntimeError("sieve too short")
        if j == i:          # even the single prime exceeds bound: nothing further
            break
    return found, tested, len(primes)


def naive(n_max):
    """Loop over n; factor C(n,2) = n(n-1)/2 by factoring n and n-1 separately."""
    sols = []
    for n in range(4, n_max + 1):
        fs = factor_td(n) + factor_td(n - 1)
        fs.remove(2)                      # divide by 2 (n(n-1) is even)
        assert math.prod(fs) == n * (n - 1) // 2
        if is_consecutive_prime_product(None, fs):
            sols.append(n)
    return sols


def main():
    n1 = int(sys.argv[1]) if len(sys.argv) > 1 else 10**8
    n2 = int(sys.argv[2]) if len(sys.argv) > 2 else 10**6
    expected = [4, 6, 15, 21, 715]

    t = time.time()
    found, tested, nprimes = block_search(n1)
    t1 = time.time() - t
    ns = sorted(n for n, _, _ in found)
    print(f"Part 1: block search, n <= {n1}: {tested} blocks, {nprimes} primes sieved, {t1:.1f}s")
    for n, a, b in sorted(found):
        m = n * (n - 1) // 2
        ok = is_consecutive_prime_product(m)
        print(f"  n = {n}: C(n,2) = {m} = block {a}..{b}; factorization {factor_td(m)}; "
              f"consecutive check: {ok}")
    print(f"  solutions {ns}  ->  {'PASS' if ns == expected else 'FAIL'}")

    t = time.time()
    nv = naive(n2)
    t2 = time.time() - t
    print(f"Part 2: naive loop over n, n <= {n2}: solutions {nv}, {t2:.1f}s  ->  "
          f"{'PASS' if nv == expected else 'FAIL'}")


if __name__ == "__main__":
    main()
