#ifndef rock_scope_hh
#define rock_scope_hh

#include <string>
#include <map>

#include "rock/exception.hh"
#include "rock/result.hh"

namespace rock {

struct Scope final {
  Scope *const parent;
  std::map<std::string, Reference> bindings;
  Scope(): Scope(nullptr) {}
  Scope(Scope *p): parent(p) {}
  Result get(const std::string& name) const {
    auto pair = bindings.find(name);
    if (pair != bindings.end()) {
      return Result(NORMAL, pair->second);
    }
    if (!parent) {
      return Result(EXCEPTION, new Exception("No such variable: " + name));
    }
    return parent->get(name);
  }
  Result set(const std::string& name, Reference value) {
    auto pair = bindings.find(name);
    if (pair != bindings.end()) {
      bindings[name] = value;
      return Result(NORMAL, value);
    }
    if (!parent) {
      return Result(EXCEPTION, new Exception("No such variable: " + name));
    }
    return parent->set(name, value);
  }
  Result declare(const std::string& name, Reference value) {
    auto pair = bindings.find(name);
    if (pair != bindings.end()) {
      return Result(
          EXCEPTION, new Exception("Variable already declared: " + name));
    }
    bindings[name] = value;
    return Result(NORMAL, value);
  }
};

}

#endif//rock_scope_hh
