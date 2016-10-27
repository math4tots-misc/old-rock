#ifndef rock_function_hh
#define rock_function_hh

#include <string>
#include <functional>

#include "rock/value.hh"
#include "rock/class.hh"

namespace rock {

struct Class;

extern Class *classFunction;

struct Function final: Value {
  std::function<Result(const Args&)> function;
  Function(std::function<Result(const Args&)> f): function(f) {}
  Class *getClass() override { return classFunction; }
};

}

#endif//rock_function_hh
