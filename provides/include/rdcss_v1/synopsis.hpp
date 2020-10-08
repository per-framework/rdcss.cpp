#pragma once

#include "polyfill_v1/type_traits.hpp"

namespace rdcss_v1 {

// casn.hpp ====================================================================

/// Multi-word atomic compare and set (CASN)
struct CASN {
  // Mutable atoms -------------------------------------------------------------

  /// A mutable atom that supports CASN operations.
  template <class Value> struct atom_t;

  /// Creates a mutable atom.
  template <class Value>
  static atom_t<std::remove_cvref_t<Value>> atom(Value &&value);

  /// Atomically loads and returns the current value of the atom.
  template <class Value> static Value load(const atom_t<Value> &atom);

  /// Atomically replaces the value of the atom with the desired value.
  template <class Value, class Forwardable_Value>
  static void store(atom_t<Value> &atom, Forwardable_Value &&desired);

  // Typed CASN ----------------------------------------------------------------

  /// Describes a single CAS operation.
  template <class Value> struct op_t;

  /// Specifies a single CAS operation.
  template <class Value, class Forwardable_Expected, class Forwardable_Desired>
  static op_t<Value> op(atom_t<Value> &atom,
                        Forwardable_Expected &&expected,
                        Forwardable_Desired &&desired);

  /// Attempts the described CAS operations.  Returns `true` on success and
  /// `false` on failure.
  template <class... Ops> static bool casn(Ops &&... ops);

  // Unityped CASN -------------------------------------------------------------

  /// Describes a single CAS operation in a unityped manner.
  struct op_mono_t;

  /// Creates a unityped reference to the CAS operation.
  template <class Value> static op_mono_t as_mono(op_t<Value> &op);

  /// Attempts the described unityped CAS operations.  Returns `true` on success
  /// and `false` on failure.
  static bool casn(op_mono_t *mono_ops);
};

} // namespace rdcss_v1
