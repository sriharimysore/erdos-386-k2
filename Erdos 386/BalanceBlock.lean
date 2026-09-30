import Mathlib
import «Erdos 386».Balance
import «Erdos 386».Lemma0

/-!
# The balance lemma for blocks of consecutive primes (Mathlib)

This file connects the arithmetic core in `Balance.lean` to the objects used in the upstream
statement `Erdos386.erdos_386.variants.two` (google-deepmind/formal-conjectures):
`n.choose 2 = ∏ i ∈ Finset.Ico a b, Nat.nth Nat.Prime i`.

* `Erdos386.choose2_ne_nth_prime`: for `n ≥ 4`, `n.choose 2` is never a single prime
  (Lemma 0, block length 1).
* `Erdos386.balance_block`: if `n ≥ 4` and `n.choose 2 = ∏ i ∈ Ico a b, nth Prime i` with
  `a ≥ 2` (i.e. first prime `≥ 5`), then with `p = nth Prime a`, `r = nth Prime (b - 1)`,
  `h = (b - a) / 2`:  `(2p - 1) * p ^ h ≤ p * r ^ h`   (Corollary 1′ of `phase3/LEMMA.md`).
-/

open Nat Finset

namespace Erdos386

/-- If `A * O` is a product of distinct primes `f i` (`i ∈ I`), then `A` is exactly the product
of those `f i` dividing `A`, and `O` the product of the others. -/
theorem split_prod {I : Finset ℕ} {f : ℕ → ℕ} (hf : Set.InjOn f I)
    (hp : ∀ i ∈ I, (f i).Prime) {A O : ℕ} (hA : 0 < A) (hO : 0 < O)
    (h : A * O = ∏ i ∈ I, f i) :
    ∏ i ∈ I with f i ∣ A, f i = A ∧ ∏ i ∈ I with ¬ f i ∣ A, f i = O := by
  classical
  -- a product of distinct primes, each dividing N, divides N
  have dvd_of : ∀ s : Finset ℕ, s ⊆ I → ∀ N : ℕ, (∀ i ∈ s, f i ∣ N) → ∏ i ∈ s, f i ∣ N := by
    intro s hs N hd
    have hinj : Set.InjOn f s := hf.mono (by intro x hx; exact hs hx)
    have himg := Finset.prod_image (f := fun x : ℕ => x) (s := s) (g := f) hinj
    rw [← himg]
    apply Finset.prod_primes_dvd
    · intro x hx
      obtain ⟨i, hi, rfl⟩ := Finset.mem_image.mp hx
      exact (hp i (hs hi)).prime
    · intro x hx
      obtain ⟨i, hi, rfl⟩ := Finset.mem_image.mp hx
      exact hd i hi
  have hX : ∏ i ∈ I with f i ∣ A, f i ∣ A :=
    dvd_of _ (filter_subset _ _) A (fun i hi => (mem_filter.mp hi).2)
  have hY : ∏ i ∈ I with ¬ f i ∣ A, f i ∣ O := by
    apply dvd_of _ (filter_subset _ _) O
    intro i hi
    have hi' := mem_filter.mp hi
    have hdvd : f i ∣ A * O := h ▸ Finset.dvd_prod_of_mem f hi'.1
    exact ((hp i hi'.1).dvd_mul.mp hdvd).resolve_left hi'.2
  have hXY : (∏ i ∈ I with f i ∣ A, f i) * (∏ i ∈ I with ¬ f i ∣ A, f i) = A * O := by
    rw [h]; exact Finset.prod_filter_mul_prod_filter_not I _ f
  have hXA : ∏ i ∈ I with f i ∣ A, f i = A := by
    rcases (Nat.le_of_dvd hA hX).lt_or_eq with hlt | heq
    · exfalso
      have : (∏ i ∈ I with f i ∣ A, f i) * (∏ i ∈ I with ¬ f i ∣ A, f i) < A * O :=
        calc _ ≤ (∏ i ∈ I with f i ∣ A, f i) * O := Nat.mul_le_mul_left _ (Nat.le_of_dvd hO hY)
          _ < A * O := Nat.mul_lt_mul_of_pos_right hlt hO
      omega
    · exact heq
  refine ⟨hXA, ?_⟩
  rw [hXA] at hXY
  exact Nat.eq_of_mul_eq_mul_left hA hXY

/-- Lemma 0 in upstream form: for `n ≥ 4`, `n.choose 2` is not a single prime `nth Prime a`. -/
theorem choose2_ne_nth_prime {n : ℕ} (hn : 4 ≤ n) (a : ℕ) : n.choose 2 ≠ nth Nat.Prime a := by
  intro h
  obtain ⟨x, y, hx, hy, hxy⟩ := choose2_split n hn
  have hp := prime_nth_prime a
  rw [← h, Nat.choose_two_right, hxy] at hp
  exact Nat.not_prime_mul (by omega) (by omega) hp

/-- **Balance lemma for blocks (Corollary 1′), in upstream form.** -/
theorem balance_block {n a b : ℕ} (hn : 4 ≤ n) (ha : 2 ≤ a)
    (h : n.choose 2 = ∏ i ∈ Finset.Ico a b, nth Nat.Prime i) :
    (2 * nth Nat.Prime a - 1) * nth Nat.Prime a ^ ((b - a) / 2) ≤
      nth Nat.Prime a * nth Nat.Prime (b - 1) ^ ((b - a) / 2) := by
  classical
  set f : ℕ → ℕ := nth Nat.Prime with hfdef
  have hmono : StrictMono f := nth_strictMono infinite_setOfPred_prime
  have hp5 : 5 ≤ f a := by
    rw [← nth_prime_two_eq_five]; exact hmono.monotone ha
  -- write C(n,2) = A * O with {2A, O} = {n, n-1} (in some order), A ≥ 2, O ≥ 3
  obtain ⟨A, O, hAO, hbal, hA2, hO3⟩ : ∃ A O, A * O = n.choose 2 ∧
      (2 * A = O + 1 ∨ O = 2 * A + 1) ∧ 2 ≤ A ∧ 3 ≤ O := by
    rw [Nat.choose_two_right]
    rcases Nat.even_or_odd n with ⟨m, hm⟩ | ⟨m, hm⟩
    · refine ⟨m, n - 1, ?_, Or.inl (by omega), by omega, by omega⟩
      subst hm
      rw [show (m + m) * (m + m - 1) = 2 * (m * (m + m - 1)) by ring]
      exact (Nat.mul_div_cancel_left _ (by norm_num)).symm
    · refine ⟨m, n, ?_, Or.inr (by omega), by omega, by omega⟩
      subst hm
      rw [show (2 * m + 1) * (2 * m + 1 - 1) = 2 * (m * (2 * m + 1)) by
        rw [Nat.add_sub_cancel]; ring]
      exact (Nat.mul_div_cancel_left _ (by norm_num)).symm
  have hAO' : A * O = ∏ i ∈ Ico a b, f i := hAO.trans h
  have hinj : Set.InjOn f (Ico a b : Finset ℕ) := hmono.injective.injOn
  have hprime : ∀ i ∈ Ico a b, (f i).Prime := fun i _ => prime_nth_prime i
  obtain ⟨hSA, hTO⟩ := split_prod hinj hprime (by omega) (by omega) hAO'
  -- the two lists of primes
  set S := (Ico a b).filter (fun i => f i ∣ A) with hS
  set T := (Ico a b).filter (fun i => ¬ f i ∣ A) with hT
  have hLS : (S.toList.map f).prod = A := by rw [Finset.prod_map_toList]; exact hSA
  have hLT : (T.toList.map f).prod = O := by rw [Finset.prod_map_toList]; exact hTO
  have inrange : ∀ U : Finset ℕ, U ⊆ Ico a b → ∀ x ∈ U.toList.map f, f a ≤ x ∧ x ≤ f (b - 1) := by
    intro U hU x hx
    obtain ⟨i, hi, rfl⟩ := List.mem_map.mp hx
    have hiI := Finset.mem_Ico.mp (hU (Finset.mem_toList.mp hi))
    exact ⟨hmono.monotone hiI.1, hmono.monotone (by omega)⟩
  have hSsub : S ⊆ Ico a b := filter_subset _ _
  have hTsub : T ⊆ Ico a b := filter_subset _ _
  have hSne : S.toList.map f ≠ [] := by
    intro hnil; rw [hnil, List.prod_nil] at hLS; omega
  have hTne : T.toList.map f ≠ [] := by
    intro hnil; rw [hnil, List.prod_nil] at hLT; omega
  have key := balance_core (f a) (f (b - 1)) hp5 (S.toList.map f) (T.toList.map f)
    (inrange S hSsub) (inrange T hTsub) hSne hTne (by rw [hLS, hLT]; exact hbal)
  -- lengths: |S| + |T| = b - a, so min |S| |T| ≤ (b - a) / 2
  have hcard : S.card + T.card = b - a := by
    rw [hS, hT, card_filter_add_card_filter_not, Nat.card_Ico]
  have hmin : min (S.toList.map f).length (T.toList.map f).length ≤ (b - a) / 2 := by
    simp only [List.length_map, Finset.length_toList]; omega
  -- p ≤ r, from any element of the (nonempty) first list
  have hpr : f a ≤ f (b - 1) := by
    obtain ⟨x, hx⟩ := List.exists_mem_of_ne_nil _ hSne
    exact le_trans (inrange S hSsub x hx).1 (inrange S hSsub x hx).2
  exact balance_mono _ _ _ _ hpr hmin key

end Erdos386
