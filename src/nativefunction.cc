#include "rock/all.hh"

namespace rock {

NativeFunction::NativeFunction(
    const std::string &n,
    std::function<Result(const Args&)> f):
        Function(n), function(f) {}

std::string NativeFunction::debug() const {
  return "<NativeFunction " + name + ">";
}

Result NativeFunction::invoke(const Args &args) {
  return function(args);
}

}
