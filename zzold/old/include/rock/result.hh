#ifndef rock_result_hh
#define rock_result_hh

#include <string>

#include "rock/reference.hh"
#include "rock/value.hh"

namespace rock {

constexpr long NORMAL = 1;
constexpr long EXCEPTION = 2;
constexpr long RETURN = 3;
constexpr long BREAK = 4;
constexpr long CONTINUE = 5;

/** The Result type is the result of any calculation performed in Rock.
 * We don't return just the Reference because there may be additional
 * context associated with it, e.g. whether we are returning an EXCEPTION.
 * If the result is just the value itself, type should be NORMAL.
 */
struct Result final {
  const long type;
  const Reference value;
  Result(long t, Reference r): type(t), value(r) {}

  std::string debugstr() const {
    return "Result(" + typeToString(type) + ", " + value.debugstr() + ")";
  }

  static std::string typeToString(long t) {
    switch (t) {
      case NORMAL: return "NORMAL";
      case EXCEPTION: return "EXCEPTION";
      case RETURN: return "RETURN";
      case BREAK: return "BREAK";
      case CONTINUE: return "CONTINUE";
      default: return "<%%%%>";
    }
  }
};

}

#endif//rock_result_hh
