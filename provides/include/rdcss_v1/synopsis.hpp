#pragma once

#include "polyfill_v1/type_traits.hpp"

namespace rdcss_v1 {

// casn.hpp ====================================================================

// Multi-word atomic compare and set (CASN)
struct CASN {
  // Mutable atoms -------------------------------------------------------------

  template <class Value> struct atom_t;
  // A mutable atom that supports CASN operations.

  template <class Value>
  static atom_t<std::remove_cvref_t<Value>> atom(Value &&value);
  // Creates a mutable atom.

  template <class Value> static Value load(const atom_t<Value> &atom);
  // Atomically loads and returns the current value of the atom.

  template <class Value, class Forwardable_Value>
  static void store(atom_t<Value> &atom, Forwardable_Value &&desired);
  // Atomically replaces the value of the atom with the desired value.

  // Typed CASN ----------------------------------------------------------------

  template <class Value> struct op_t;
  // Describes a single CAS operation.

  template <class Value, class Forwardable_Expected, class Forwardable_Desired>
  static op_t<Value> op(atom_t<Value> &atom, Forwardable_Expected &&expected,
                        Forwardable_Desired &&desired);
  // Specifies a single CAS operation.

  template <class... Ops> static bool casn(Ops &&... ops);
  // Attempts the described CAS operations.  Returns `true` on success and
  // `false` on failure.

  // Unityped CASN -------------------------------------------------------------

  struct op_mono_t;
  // Describes a single CAS operation in a unityped manner.

  template <class Value> static op_mono_t as_mono(op_t<Value> &op);
  // Creates a unityped reference to the CAS operation.

  static bool casn(op_mono_t *mono_ops);
  // Attempts the described unityped CAS operations.  Returns `true` on success
  // and `false` on failure.
};

} // namespace rdcss_v1
