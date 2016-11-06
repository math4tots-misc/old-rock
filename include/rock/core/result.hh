#ifndef rock_core_result_hh
#define rock_core_result_hh

#include <functional>
#include <vector>

#include "rock/core/reference.hh"

namespace rock {

class Result final {
public:
  enum class Type {
    OK, EXCEPTION, RETURN, BREAK, CONTINUE,
  };
  Type type;
  Reference value;
  Result(Reference);
  Result(Type, Reference);
  Reference get() const;
  std::string debug() const;
};

}  // namespace rock

namespace std {

std::string to_string(rock::Result::Type);

}  // namespace std

#endif//rock_core_result_hh
