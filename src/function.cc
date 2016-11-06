#include "rock/all.hh"

namespace rock {

Class *classFunction;

namespace {
Init init(110, __FILE__, []() {
  classFunction = new Class("Function", {classObject}, {
    {"__call", [](const Reference &owner, const Args &args) {
      return owner.as<Function>()->invoke(args);
    }},
  });
  builtins->declare("Function", classFunction);
});
}

Function::Function(const std::string &n): name(n) {}

Reference Function::getClass() const {
  return classFunction;
}

std::string Function::debug() const {
  return "<Function " + name + ">";
}

Reference Function::from(
    const std::string &name, std::function<Result(const Args&)> f) {
  return new NativeFunction(name, f);
}

}
