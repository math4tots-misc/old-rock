#include "rock/all.hh"

namespace rock {

Class *classFunction;

namespace {
Init init(10, __FILE__, []() {
  classFunction = new Class("Function", {
    {"__call", [](const Reference &owner, const Args &args) {
      return owner.as<Function>()->function(args);
    }},
  });
  builtins->declare("Function", classFunction);
});
}

Function::Function(
    const std::string &n,
    std::function<Result(const Args&)> f): name(n), function(f) {}

Reference Function::getClass() const {
  return classFunction;
}

std::string Function::debug() const {
  return "<Function " + name + ">";
}

}
