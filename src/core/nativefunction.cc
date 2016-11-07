#include "rock/core/all.hh"

namespace rock {

NativeFunction::NativeFunction(
    const std::string &n,
    std::function<Reference(const Args&)> f):
        Function(n), function(f) {}

std::string NativeFunction::debug() const {
  return "<NativeFunction " + name + ">";
}

Reference NativeFunction::invoke(const Args &args) {
  return function(args);
}

}
