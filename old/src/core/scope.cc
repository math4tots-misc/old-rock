#include "rock/core/all.hh"

#include <iostream>

namespace rock {

Class *classScope;

namespace {
Init init(110, __FILE__, []() {
  classScope = new Class("Scope", {classObject}, {});
  builtins->declare("Scope", classScope);
});
}  // namespace

Scope::Scope(): Scope(nullptr) {}

Scope::Scope(Scope *p): parent(p) {};

Reference Scope::get(const std::string &name) {
  auto entry = bindings.find(name);
  if (entry != bindings.end()) {
    return entry->second;
  }
  if (parent.is_not_null()) {
    return parent.as<Scope>()->get(name);
  }
  throw exception("No such variable (get): '" + name + "'");
}

Reference Scope::set(const std::string &name, Reference value) {
  auto entry = bindings.find(name);
  if (entry != bindings.end()) {
    bindings[name] = value;
    return value;
  }
  if (parent.is_not_null()) {
    return parent.as<Scope>()->set(name, value);
  }
  throw exception("No such variable (set): '" + name + "'");
}

Reference Scope::declare(const std::string &name) {
  return declare(name, nil);
}

Reference Scope::declare(const std::string &name, Reference value) {
  auto entry = bindings.find(name);
  if (entry == bindings.end()) {
    bindings[name] = value;
    return value;
  }
  throw exception("Variable already declared: '" + name + "'");
}

void Scope::erase(const std::string &name) {
  bindings.erase(name);
}

Reference Scope::getClass() const {
  return classScope;
}

std::map<std::string,Reference> Scope::getBindings() const {
  return bindings;
}

}
