#!/usr/bin/env python3
"""Tail part of the block-length theorem (see LEMMA.md, 'Block-length theorem').
For a solution whose block starts at p >= X0 (with k primes), the gap theorem gives
(r/p)^h <= exp(eps(X0) * (k-1) * floor(k/2)). Lemma 1 needs this to be >= 2 - 1/p
>= 2 - 1/X0. So the smallest possible k is the least k with
    eps(X0) * (k-1) * floor(k/2) >= ln(2 - 1/X0).
Computed with 60-digit Decimal arithmetic."""
from decimal import Decimal as Dm, getcontext
getcontext().prec = 60
X0 = Dm(10726905041)
lnX = X0.ln()
target = (2 - 1 / X0).ln()
thms = {
    "Dusart 2010 Prop 6.8 (x>=396738, eps=1/(25 ln^2 x))": 1 / (25 * lnX**2),
    "Dusart 2018 Cor 5.5 (x>=468991632, eps=1/(5000 ln^2 x))": 1 / (5000 * lnX**2),
    "Ramare-Saouter 2003 Thm 3 (x>=10726905041, eps=1/28313999)": 1 / Dm(28313999),
}
for name, eps in thms.items():
    k = 2
    while eps * (k - 1) * (k // 2) < target:
        k += 1
    print(f"{name}: a solution with start >= X0 needs k >= {k}")
