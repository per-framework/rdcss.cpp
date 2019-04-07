#include "rdcss_v1/casn.hpp"

using namespace rdcss_v1;

CASN_detail::content_t *
CASN_detail::cas1(atom_base_t &atom, content_t *expected, content_t *desired) {
  atom.content.compare_exchange_strong(expected, desired);
  return expected;
}

bool CASN_detail::is_rdcss(content_t *content) {
  return content->type == RDCSS;
}

CASN_detail::content_t *CASN_detail::rdcss(rdcss_t &descriptor) {
  atom_base_t &atom = descriptor.atom;
  content_t *expected = descriptor.expected;
  content_t *desired = descriptor.desired;
  while (true) {
    content_t *result = cas1(atom, expected, &descriptor);
    if (result == expected) {
      cas1(atom, &descriptor,
           descriptor.status == UNDECIDED ? desired : expected);
      return result;
    } else if (is_rdcss(result)) {
      rdcss_complete(atom, *static_cast<rdcss_t *>(result));
    } else {
      return result;
    }
  }
}

void CASN_detail::rdcss_complete(atom_base_t &atom, rdcss_t &descriptor) {
  cas1(atom, &descriptor,
       descriptor.status == UNDECIDED ? descriptor.desired
                                      : descriptor.expected);
}

CASN_detail::content_t *CASN_detail::rdcss_read(atom_base_t &atom) {
  while (true) {
    content_t *result = atom.content;
    if (is_rdcss(result))
      rdcss_complete(atom, *static_cast<rdcss_t *>(result));
    else
      return result;
  }
}

bool CASN_detail::is_casn(content_t *content) { return content->type == CASN; }

bool CASN_detail::casn(casn_t &descriptor) {
  op_mono_t *ops = descriptor.ops;

  if (descriptor.status == UNDECIDED) {
    status_t status = SUCCEEDED;

    do {
      op_mono_t &entry = *ops;
      atom_base_t *atom = entry.atom;
      if (!atom)
        break;
      ++ops;
      content_t *before = entry.before;
      rdcss_t op = {{RDCSS}, descriptor.status, *atom, before, &descriptor};
      while (true) {
        content_t *result = rdcss(op);
        if (result == before || result == &descriptor) {
          break;
        } else if (is_casn(result)) {
          casn(*static_cast<casn_t *>(result));
        } else {
          status = FAILED;
          break;
        }
      }
    } while (status == SUCCEEDED);

    status_t undecided = UNDECIDED;
    descriptor.status.compare_exchange_strong(undecided, status);
  }

  bool succeeded = descriptor.status == SUCCEEDED;

  op_mono_t *ops_base = descriptor.ops;
  while (ops_base < ops) {
    op_mono_t &entry = *--ops;
    cas1(*entry.atom, &descriptor, succeeded ? entry.after : entry.before);
  }

  return succeeded;
}

CASN_detail::content_t *CASN_detail::casn_read(atom_base_t &atom) {
  while (true) {
    content_t *result = rdcss_read(atom);
    if (is_casn(result))
      casn(*static_cast<casn_t *>(result));
    else
      return result;
  }
}

bool CASN::casn(op_mono_t *ops) {
  CASN_detail::casn_t descriptor = {
      {CASN_detail::CASN}, {CASN_detail::UNDECIDED}, ops};
  return CASN_detail::casn(descriptor);
}
