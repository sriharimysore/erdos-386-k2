/-!
# The balance lemma (arithmetic core), Lean 4 core only — no Mathlib

If `2A` and `O` differ by 1, `A` is a product of `s ≥ 1` factors and `O` a product of
`t ≥ 1` factors, and all factors lie in `[p, r]` with `p ≥ 5`, then

    p * r ^ h ≥ (2 * p - 1) * p ^ h,   where h = min s t.

This is Lemma 1 of `phase3/LEMMA.md` with the number theory stripped away (the split of the
block's primes into the prime factors of `A = E/2` and `O` is the Mathlib-dependent part).
-/

namespace Erdos386

/-- Bounds on a product of factors lying in `[p, r]`. -/
theorem prod_bounds (p r : Nat) :
    ∀ l : List Nat, (∀ x ∈ l, p ≤ x ∧ x ≤ r) →
      p ^ l.length ≤ l.prod ∧ l.prod ≤ r ^ l.length
  | [], _ => by simp
  | x :: l, h => by
    have hx := h x (by simp)
    have ih := prod_bounds p r l (fun y hy => h y (by simp [hy]))
    simp only [List.prod_cons, List.length_cons, Nat.pow_succ]
    constructor
    · rw [Nat.mul_comm (p ^ l.length) p]
      exact Nat.mul_le_mul hx.1 ih.1
    · rw [Nat.mul_comm (r ^ l.length) r]
      exact Nat.mul_le_mul hx.2 ih.2

/-- Case `s = t`. From `A ≥ p^s`, `O ≤ r^s`, `O + 1 ≥ 2A`, `A ≥ p`. -/
theorem balance_eq (p r A O s : Nat) (hp : 1 ≤ p) (hA : p ^ s ≤ A) (hO : O ≤ r ^ s)
    (hbal : 2 * A ≤ O + 1) (hAp : p ≤ A) :
    (2 * p - 1) * p ^ s ≤ p * r ^ s := by
  -- (2A - 1) * p^s ≤ O * p^s ≤ r^s * A
  have h1 : (2 * A - 1) * p ^ s ≤ r ^ s * A := by
    calc (2 * A - 1) * p ^ s ≤ O * p ^ s := Nat.mul_le_mul_right _ (by omega)
      _ ≤ r ^ s * A := Nat.mul_le_mul hO hA
  -- A * (2p - 1) ≤ p * (2A - 1)  since A ≥ p
  have h2 : A * (2 * p - 1) ≤ p * (2 * A - 1) := by
    have : 1 ≤ p := by omega
    have : 1 ≤ A := by omega
    rw [Nat.mul_sub, Nat.mul_sub, Nat.mul_one, Nat.mul_one]
    have e : A * (2 * p) = p * (2 * A) := by
      rw [Nat.mul_comm 2 p, Nat.mul_comm 2 A, ← Nat.mul_assoc, Nat.mul_comm A p, Nat.mul_assoc]
    omega
  have hApos : 0 < A := by
    have : 0 < p := by omega
    omega
  -- A * ((2p-1) p^s) ≤ p * (2A-1) * p^s ≤ p * (r^s * A) = A * (p r^s)
  have h3 : A * ((2 * p - 1) * p ^ s) ≤ A * (p * r ^ s) := by
    calc A * ((2 * p - 1) * p ^ s) = (A * (2 * p - 1)) * p ^ s := by rw [Nat.mul_assoc]
      _ ≤ (p * (2 * A - 1)) * p ^ s := Nat.mul_le_mul_right _ h2
      _ = p * ((2 * A - 1) * p ^ s) := by rw [Nat.mul_assoc]
      _ ≤ p * (r ^ s * A) := Nat.mul_le_mul_left _ h1
      _ = A * (p * r ^ s) := by
        rw [← Nat.mul_assoc, Nat.mul_comm p (r ^ s), Nat.mul_comm (r ^ s * p) A]
  exact Nat.le_of_mul_le_mul_left h3 hApos

/-- Pure inequality used when `s < t`: from `p X ≤ 2Y + 1`, `X ≥ 1`, `p ≥ 5`. -/
theorem ineq_lt (p X Y : Nat) (hp : 5 ≤ p) (hX : 1 ≤ X) (h : p * X ≤ 2 * Y + 1) :
    (2 * p - 1) * X ≤ p * Y := by
  have a1 : 5 * (p * X) ≤ p * (p * X) := Nat.mul_le_mul_right _ hp
  have a2 : p * 1 ≤ p * X := Nat.mul_le_mul_left _ hX
  have a3 : p * (p * X) ≤ p * (2 * Y + 1) := Nat.mul_le_mul_left _ h
  have a4 : p * (2 * Y + 1) = 2 * (p * Y) + p := by
    rw [Nat.mul_add, Nat.mul_one, ← Nat.mul_assoc, Nat.mul_comm p 2, Nat.mul_assoc]
  have a5 : (2 * p - 1) * X = 2 * (p * X) - X := by
    rw [Nat.sub_mul, Nat.one_mul, Nat.mul_assoc]
  have a6 : X ≤ p * X := by
    have := Nat.mul_le_mul_right X (show 1 ≤ p by omega); simpa using this
  rw [a5]; omega

/-- Pure inequality used when `s > t`: from `2 p X ≤ Y + 1`, `X ≥ 1`, `p ≥ 5`. -/
theorem ineq_gt (p X Y : Nat) (hp : 5 ≤ p) (hX : 1 ≤ X) (h : 2 * (p * X) ≤ Y + 1) :
    (2 * p - 1) * X ≤ p * Y := by
  have a1 : p * (2 * (p * X)) ≤ p * (Y + 1) := Nat.mul_le_mul_left _ h
  have a2 : p * 1 ≤ p * X := Nat.mul_le_mul_left _ hX
  have a3 : 5 * (p * X) ≤ p * (p * X) := Nat.mul_le_mul_right _ hp
  have a4 : p * (2 * (p * X)) = 2 * (p * (p * X)) := by
    rw [← Nat.mul_assoc, Nat.mul_comm p 2, Nat.mul_assoc]
  have a4' : p * (Y + 1) = p * Y + p := by rw [Nat.mul_add, Nat.mul_one]
  have a5 : (2 * p - 1) * X = 2 * (p * X) - X := by
    rw [Nat.sub_mul, Nat.one_mul, Nat.mul_assoc]
  rw [a5]; omega

/-- **Balance lemma, arithmetic core.** `A` is a product of the factors in `LS`, `O` of those
in `LT`; all factors lie in `[p, r]`, `p ≥ 5`; `A ≥ 2`-sized nonempty lists; and `2A`, `O`
differ by one. Then `(2p - 1) p^h ≤ p r^h` for `h = min |LS| |LT|`. -/
theorem balance_core (p r : Nat) (hp : 5 ≤ p) (LS LT : List Nat)
    (hS : ∀ x ∈ LS, p ≤ x ∧ x ≤ r) (hT : ∀ x ∈ LT, p ≤ x ∧ x ≤ r)
    (hSne : LS ≠ []) (hTne : LT ≠ [])
    (hbal : 2 * LS.prod = LT.prod + 1 ∨ LT.prod = 2 * LS.prod + 1) :
    (2 * p - 1) * p ^ (min LS.length LT.length) ≤ p * r ^ (min LS.length LT.length) := by
  obtain ⟨hS1, hS2⟩ := prod_bounds p r LS hS
  obtain ⟨hT1, hT2⟩ := prod_bounds p r LT hT
  have hp1 : 1 ≤ p := by omega
  have hsl : 1 ≤ LS.length := by
    cases LS with
    | nil => exact absurd rfl hSne
    | cons _ _ => simp
  have htl : 1 ≤ LT.length := by
    cases LT with
    | nil => exact absurd rfl hTne
    | cons _ _ => simp
  have powpos : ∀ n, 1 ≤ p ^ n := fun n => Nat.pow_pos (by omega)
  -- A ≥ p because LS is nonempty
  have hAp : p ≤ LS.prod := by
    calc p = p ^ 1 := (Nat.pow_one p).symm
      _ ≤ p ^ LS.length := Nat.pow_le_pow_right hp1 hsl
      _ ≤ LS.prod := hS1
  rcases Nat.lt_trichotomy LS.length LT.length with hlt | heq | hgt
  · -- s < t, h = s
    rw [Nat.min_eq_left (Nat.le_of_lt hlt)]
    apply ineq_lt p _ _ hp (powpos _)
    calc p * p ^ LS.length = p ^ (LS.length + 1) := by rw [Nat.pow_succ, Nat.mul_comm]
      _ ≤ p ^ LT.length := Nat.pow_le_pow_right hp1 hlt
      _ ≤ LT.prod := hT1
      _ ≤ 2 * LS.prod + 1 := by omega
      _ ≤ 2 * r ^ LS.length + 1 := by omega
  · -- s = t
    rw [heq, Nat.min_self]
    exact balance_eq p r LS.prod LT.prod LT.length hp1 (heq ▸ hS1) hT2 (by omega) hAp
  · -- s > t, h = t
    rw [Nat.min_eq_right (Nat.le_of_lt hgt)]
    apply ineq_gt p _ _ hp (powpos _)
    calc 2 * (p * p ^ LT.length) = 2 * p ^ (LT.length + 1) := by rw [Nat.pow_succ, Nat.mul_comm p]
      _ ≤ 2 * p ^ LS.length := Nat.mul_le_mul_left _ (Nat.pow_le_pow_right hp1 hgt)
      _ ≤ 2 * LS.prod := Nat.mul_le_mul_left _ hS1
      _ ≤ LT.prod + 1 := by omega
      _ ≤ r ^ LT.length + 1 := by omega

/-- Raising the exponent preserves the inequality (since `p ≤ r`). -/
theorem balance_mono (p r h h' : Nat) (hpr : p ≤ r) (hh : h ≤ h')
    (H : (2 * p - 1) * p ^ h ≤ p * r ^ h) :
    (2 * p - 1) * p ^ h' ≤ p * r ^ h' := by
  obtain ⟨d, rfl⟩ := Nat.exists_eq_add_of_le hh
  rw [Nat.pow_add, Nat.pow_add, ← Nat.mul_assoc, ← Nat.mul_assoc]
  exact Nat.mul_le_mul H (Nat.pow_le_pow_left hpr d)

end Erdos386
