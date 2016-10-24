#ifndef rock_result_hh
#define rock_result_hh

#include <string>

#include "rock/value/value.hh"

namespace rock {

constexpr long NORMAL = 1;
constexpr long EXCEPTION = 2;
constexpr long RETURN = 3;
constexpr long BREAK = 4;
constexpr long CONTINUE = 5;

struct Result {
  const long type;
  const P value;

  Result(long t): Result(t, P()) {}
  Result(long t, P p): type(t), value(p) {}
};

}

#endif//rock_result_hh
