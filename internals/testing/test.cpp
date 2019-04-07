#include "rdcss_v1/casn.hpp"

#include "testing_v1/test.hpp"

using namespace testing_v1;

auto test_casn = test([]() {
  using namespace rdcss_v1;

  auto i = CASN::atom(101);
  auto b = CASN::atom(false);
  CASN::store(b, true);
  verify(CASN::load(i) == 101);
  verify(CASN::load(b) == true);

  verify(!CASN::casn(CASN::op(i, 101, 69), CASN::op(b, false, true)));
  verify(CASN::load(i) == 101);
  verify(CASN::load(b) == true);

  verify(CASN::casn(CASN::op(i, 101, 69), CASN::op(b, true, false)));
  verify(CASN::load(i) == 69);
  verify(CASN::load(b) == false);
});
