#ifndef rock_value_exception_hh
#define rock_value_exception_hh

#include "rock/stack.hh"
#include "rock/value/class.hh"

namespace rock {

Class classException("Exception");  // TODO: cc

struct Exception final: Value {
  const std::string message;
  const std::vector<const Token*> stacktrace;

  Exception(const std::string& m):
      message(m), stacktrace(stack.begin(), stack.end()) {}

  Class& getClass() const override { return classException; }
  size_t hash() override { return reinterpret_cast<size_t>(this); }
};

}

#endif//rock_value_exception_hh
