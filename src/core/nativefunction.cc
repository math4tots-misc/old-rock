#include "rock/core/all.hh"

namespace rock {

NativeFunction::NativeFunction(
    const std::string &n,
    Reference(*f)(const Args&)):
        Function(n), function(f) {}

std::string NativeFunction::debug() const {
  return "<NativeFunction " + name + ">";
}

Reference NativeFunction::invoke(const Args &args) {
  return function(args);
}

}
