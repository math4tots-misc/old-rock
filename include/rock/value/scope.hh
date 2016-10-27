#ifndef rock_value_scope_hh
#define rock_value_scope_hh

#include <string>
#include <map>

#include "rock/value/value.hh"
#include "rock/value/exception.hh"

namespace rock {

struct Scope final {
  Scope *const parent;
  std::map<std::string, Ref> bindings;
  Scope(): Scope(nullptr) {}
  Scope(Scope *p): parent(p) {}
  Result<Ref> get(const std::string& name) const {
    auto pair = bindings.find(name);
    if (pair != bindings.end()) {
      return Result<Ref>(NORMAL, pair->second);
    }
    if (!parent) {
      return Result<Ref>(
          EXCEPTION, new Exception("No such variable: " + name));
    }
    return parent->get(name);
  }
  Result<Ref> set(const std::string& name, Ref value) {
    auto pair = bindings.find(name);
    if (pair != bindings.end()) {
      bindings[name] = value;
      return Result<Ref>(NORMAL, value);
    }
    if (!parent) {
      return Result<Ref>(
          EXCEPTION, new Exception("No scuh variable: " + name));
    }
    return parent->set(name, value);
  }
  Result<Ref> declare(const std::string& name, Ref value) {
    auto pair = bindings.find(name);
    if (pair != bindings.end()) {
      return Result<Ref>(
          EXCEPTION, new Exception("Variable already declared: " + name));
    }
    bindings[name] = value;
    return Result<Ref>(NORMAL, value);
  }
};

}

#endif//rock_value_scope_hh
