#!/usr/bin/env python3
"""For each explicit prime-gap theorem, the largest D such that the tail argument
(LEMMA.md) covers N = 10^D. High-precision Decimal arithmetic, rounded conservatively.

Tail condition: with X = 10,726,905,041 (the computational part covers every start
below it), kmax = floor(ln M / ln X), and M < 10^(2D), we need
    eps(X) * kmax*(kmax-1)/2 < ln(1.99)   (<= ln(2 - 1/p) for p >= 100).
"""
from decimal import Decimal as Dm, getcontext
getcontext().prec = 60
X = Dm(10726905041)
lnX = X.ln()
LN199 = Dm("1.99").ln()
ln10 = Dm(10).ln()
thms = {
    "Dusart 2010 Prop 6.8   eps=1/(25 ln^2 x)":   1 / (25 * lnX * lnX),
    "Dusart 2018 Cor 5.5    eps=1/(5000 ln^2 x)": 1 / (5000 * lnX * lnX),
    "Ramare-Saouter 2003 Thm 3  eps=1/28313999":  1 / Dm(28313999),
}
def ok(D, eps):
    lnM = 2 * D * ln10            # M < N^2 = 10^(2D)
    kmax = int(lnM / lnX)         # floor; p^k <= M forces k <= kmax
    return eps * kmax * (kmax - 1) / 2 < LN199
for name, eps in thms.items():
    lo, hi = 1, 10**7
    while lo < hi:
        mid = (lo + hi + 1) // 2
        if ok(mid, eps): lo = mid
        else: hi = mid - 1
    print(f"{name:45s}  tail covers N = 10^D for all D <= {lo}")
