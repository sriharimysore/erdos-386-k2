**$k=2$: making StijnC's argument explicit, and a bounded verification. Not a solution.**

Building on @StijnC's observation (24 Aug 2025) that $2\prod_{I} p_i$ and $\prod_{J} p_j$ must differ by exactly $1$, so that each fixed block length allows only finitely many solutions, I tried to make this fully explicit and push it as far as possible. Code, logs and proofs: https://github.com/sriharimysore/erdos-386-k2

**1. A quantitative form of the balance argument.**

*Statement.* If $n\ge 4$ and every prime factor of $\binom{n}{2}$ lies in $[p,r]$ with $p\ge 5$, then
$$\left(\frac{r}{p}\right)^{\lfloor k/2\rfloor}\ \ge\ 2-\frac{1}{p},\qquad k=\Omega\!\left(\tbinom{n}{2}\right).$$

*Proof idea.* Write $\binom{n}{2}=A\cdot O$ with $O=2A\pm 1$ ($A=E/2$, where $E$ and $O$ are the even and odd members of $\{n,n-1\}$). Split the primes as $A=\prod S$ and $O=\prod T$. There are three cases: $|S|=|T|$, $|S|<|T|$, $|S|>|T|$.

*Sharpness.* $n=14$: $\binom{14}{2}=91=7\cdot 13$ and $13/7=2-1/7$. This example is not a consecutive block.

*Lean.* The block form is machine-checked in Lean 4 + Mathlib, stated with the upstream objects
`n.choose 2 = ∏ i ∈ Finset.Ico a b, Nat.nth Nat.Prime i` ($a\ge 2$). No `sorry`, no `native_decide`.

**2. Explicit consequences.** These are conditional on one prime-gap theorem: Dusart, *Ramanujan J.* 45 (2018), Cor. 5.5. For $x\ge 468{,}991{,}632$ there is a prime in $\left(x,\ x\left(1+\frac{1}{5000\ln^2 x}\right)\right]$. I use it for starting primes $\ge 1.07\cdot 10^{10}$, with a computer search below that.

- **For all $n$**, any further solution is a product of **at least $1924$ consecutive primes**. This makes StijnC's "finitely many for each fixed length" explicit: there are none with $\le 1923$ primes.
- No solutions besides $n=4,6,15,21,715$ for **$n\le 10^{9614}$**. This extends Desmond's search ($n>10^{500}$, per StijnC's comment), and it also covers Corneth's OEIS A280992 bound (any further term has a prime factor $>17389$).

**3. Computation only** (no prime-gap input; ordinary C/Python, not formally verified).

- No further solution for $n\le 10^{12}$. Three independent programs agree up to $10^9$, and one C program was run to $10^{12}$.
- Blocks starting at any prime $<100$ and ending at a prime $\le 4\cdot 10^9$ give nothing new. In particular, $n(n-1)=r\#$ has no new solution for $r\le 4\cdot 10^9$.

**4. A remark on why the long-block case looks hard.** The $n$ with $2P\mid n(n-1)$ are $2^k$ residues mod $2P$. For blocks starting at $2$ or $3$, the least one tracks the random-model prediction $\lambda=n(n-1)/(2P)\approx P/(2\cdot 4^k)$, up to $\lambda\approx 10^{22}$ ($r\le 97$). As far as I can tell, abc gives nothing here, since $n(n-1)$ is squarefree up to the factor $2$.

**Disclosure.** This was done with substantial assistance from Claude (Anthropic), an AI model. Corrections are very welcome, especially on the explicit constants in part 2.
