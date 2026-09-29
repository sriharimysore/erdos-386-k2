#!/usr/bin/env python3
"""Independent cross-check: loop over n directly (slow, small N only).

For each 4 <= n <= N, factor C(n,2) via a smallest-prime-factor sieve and test
whether it is squarefree with prime support equal to a run of consecutive primes.
Shares no code with block_search.py.

Usage: python3 brute_check.py N
"""
import sys


def main():
    N = int(sys.argv[1])
    spf = list(range(N + 1))
    for i in range(2, int(N ** 0.5) + 1):
        if spf[i] == i:
            for j in range(i * i, N + 1, i):
                if spf[j] == j:
                    spf[j] = i
    is_p = [False, False] + [spf[i] == i for i in range(2, N + 1)]
    nxt = [0] * (N + 2)  # nxt[p] = next prime after p (0 if > N)
    last = 0
    for i in range(N, 1, -1):
        nxt[i] = last
        if is_p[i]:
            last = i

    def factor(m, out):
        while m > 1:
            p = spf[m]
            out.append(p)
            m //= p

    sols = []
    for n in range(4, N + 1):
        fs = []
        a, b = (n // 2, n - 1) if n % 2 == 0 else (n, (n - 1) // 2)
        factor(a, fs)
        factor(b, fs)
        fs.sort()
        if any(fs[i] == fs[i + 1] for i in range(len(fs) - 1)):
            continue  # not squarefree
        if all(nxt[fs[i]] == fs[i + 1] for i in range(len(fs) - 1)):
            sols.append(n)
    print(f"N = {N}: solutions {sols}")


if __name__ == "__main__":
    main()
