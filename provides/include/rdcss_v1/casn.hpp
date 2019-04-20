#pragma once

#include "rdcss_v1/config.hpp"
#include "rdcss_v1/synopsis.hpp"

#include "lax_v1/comparison.hpp"
#include "lax_v1/logical.hpp"
#include "lax_v1/type_traits.hpp"

#include <atomic>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace rdcss_v1 {

class CASN_private : CASN {
  friend struct CASN;

  template <class Value>
  struct is_stored_plain_m
      : lax::or_m<
            lax::and_m<
                lax::is_pointer_m<Value>,
                lax::lte_m<lax::auto_t<2>,
                           lax::alignment_of_m<lax::remove_pointer_m<Value>>>>,
            lax::and_m<lax::is_array_m<Value>,
                       is_stored_plain_m<lax::remove_all_extents_m<Value>>>> {};

  template <class Value>
  static inline constexpr bool is_stored_plain_v =
      lax::value_of_v<is_stored_plain_m<lax::type_t<Value>>>;

  static_assert(!is_stored_plain_v<int>);
  static_assert(is_stored_plain_v<int *>);
  static_assert(is_stored_plain_v<int * [2]>);
  static_assert(is_stored_plain_v<float * [2]>);
  static_assert(!is_stored_plain_v<float>);

  static inline constexpr size_t align_to(size_t alignment, size_t size) {
    return (size + alignment - 1) & ~(alignment - 1);
  }

  using elem_t = std::atomic<void *>;

  template <class Value>
  static inline constexpr size_t num_elems_for =
      align_to(sizeof(elem_t),
               sizeof(Value) * (is_stored_plain_v<Value> ? 1 : 2)) /
      sizeof(elem_t);

  template <class Value> struct atom_t {
  private:
    elem_t elems[num_elems_for<Value>];
  };
};

class CASN_detail : CASN {
  friend struct CASN;

  struct atom_base_t;

  struct content_t;
  enum type_t : int_fast8_t;

  template <class Value> struct value_t;

  enum status_t : int_fast8_t;
  struct rdcss_t;
  struct casn_t;

  static content_t *
  cas1(atom_base_t &atom, content_t *expected, content_t *desired);

  static bool is_rdcss(content_t *content);
  static content_t *rdcss(rdcss_t &descriptor);
  static void rdcss_complete(atom_base_t &atom, rdcss_t &descriptor);
  static content_t *rdcss_read(atom_base_t &atom);

  static bool is_casn(content_t *content);
  static bool casn(casn_t &descriptor);
  static content_t *casn_read(atom_base_t &atom);
};

} // namespace rdcss_v1

struct rdcss_v1::CASN_detail::atom_base_t {
  std::atomic<content_t *> content;
};

//

enum rdcss_v1::CASN_detail::type_t : int_fast8_t { RDCSS = -1, VALUE, CASN };

struct rdcss_v1::CASN_detail::content_t {
  type_t type;
};

enum rdcss_v1::CASN_detail::status_t : int_fast8_t {
  FAILED = -1,
  UNDECIDED,
  SUCCEEDED
};

struct rdcss_v1::CASN_detail::rdcss_t : content_t {
  std::atomic<status_t> &status; // UNDECIDED expected
  atom_base_t &atom;
  content_t *expected;
  content_t *desired;
};

struct rdcss_v1::CASN_detail::casn_t : content_t {
  std::atomic<status_t> status;
  op_mono_t *ops;
};

struct rdcss_v1::CASN::op_mono_t {
  CASN_detail::atom_base_t *atom;
  CASN_detail::content_t *before;
  CASN_detail::content_t *after;
};

template <class Value> struct rdcss_v1::CASN_detail::value_t : content_t {
  value_t(Value &&value) : content_t{VALUE}, value(std::move(value)) {}
  value_t(const Value &value) : content_t{VALUE}, value(value) {}
  // TODO ref count?
  Value value;
};

//

template <class Value>
struct rdcss_v1::CASN::atom_t : CASN_detail::atom_base_t {};

template <class Value>
rdcss_v1::CASN::atom_t<std::remove_cvref_t<Value>>
rdcss_v1::CASN::atom(Value &&value) {
  return {{new CASN_detail::value_t<std::remove_cvref_t<Value>>(
      std::forward<Value>(value))}};
}

template <class Value> Value rdcss_v1::CASN::load(const atom_t<Value> &atom) {
  CASN_detail::content_t *result =
      CASN_detail::casn_read(const_cast<atom_t<Value> &>(atom));
  return static_cast<CASN_detail::value_t<Value> *>(result)->value;
}

template <class Value, class Forwardable_Value>
void rdcss_v1::CASN::store(atom_t<Value> &atom, Forwardable_Value &&value) {
  CASN_detail::value_t<Value> *desired =
      new CASN_detail::value_t<Value>(std::forward<Forwardable_Value>(value));
  while (true) {
    CASN_detail::content_t *expected = CASN_detail::casn_read(atom);
    if (expected == CASN_detail::cas1(atom, expected, desired))
      break;
  }
}

//

template <class Value> struct rdcss_v1::CASN::op_t : op_mono_t {};

template <class Value, class Forwardable_Expected, class Forwardable_Desired>
rdcss_v1::CASN::op_t<Value> rdcss_v1::CASN::op(atom_t<Value> &atom,
                                               Forwardable_Expected &&expected,
                                               Forwardable_Desired &&desired) {
  CASN_detail::content_t *before = CASN_detail::casn_read(atom);
  CASN_detail::content_t *after = nullptr;
  if (expected == static_cast<CASN_detail::value_t<Value> *>(before)->value) {
    after = new CASN_detail::value_t<Value>(
        std::forward<Forwardable_Desired>(desired));
  } else {
    before = nullptr;
  }
  return {op_mono_t{&atom, before, after}};
}

template <class Value>
rdcss_v1::CASN::op_mono_t rdcss_v1::CASN::as_mono(op_t<Value> &op) {
  return op;
}

template <class... Ops> bool rdcss_v1::CASN::casn(Ops &&... ops) {
  op_mono_t mono_ops[] = {std::forward<Ops>(ops)...,
                          {nullptr, nullptr, nullptr}};
  return casn(mono_ops);
}
