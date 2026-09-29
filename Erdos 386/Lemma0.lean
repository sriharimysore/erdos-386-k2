/-!
# Lemma 0 (Lean 4 core, no Mathlib)

For `n ≥ 4`, `n(n-1)/2` splits as a product of two factors, each at least 2, so it is
never prime. That rules out blocks of a single prime.
-/

namespace Erdos386

theorem choose2_split (n : Nat) (h : 4 ≤ n) :
    ∃ a b, 2 ≤ a ∧ 2 ≤ b ∧ n * (n - 1) / 2 = a * b := by
  rcases Nat.mod_two_eq_zero_or_one n with h2 | h2
  · obtain ⟨m, rfl⟩ : ∃ m, n = 2 * m := ⟨n / 2, by omega⟩
    refine ⟨m, 2 * m - 1, by omega, by omega, ?_⟩
    rw [Nat.mul_assoc, Nat.mul_div_cancel_left _ (by decide : 0 < 2)]
  · obtain ⟨m, rfl⟩ : ∃ m, n = 2 * m + 1 := ⟨n / 2, by omega⟩
    refine ⟨2 * m + 1, m, by omega, by omega, ?_⟩
    rw [show 2 * m + 1 - 1 = 2 * m by omega, Nat.mul_comm 2 m, ← Nat.mul_assoc,
      Nat.mul_div_cancel _ (by decide : 0 < 2)]

end Erdos386
