#include "rock/all.hh"

namespace rock {

Class *classScope;

namespace {
Init init(110, __FILE__, []() {
  classScope = new Class("Scope");
  builtins->declare("Scope", classScope);
});
}  // namespace

Scope::Scope(): Scope(nullptr) {}

Scope::Scope(Scope *p): parent(p) {};

Result Scope::get(const std::string &name) {
  auto entry = bindings.find(name);
  if (entry != bindings.end()) {
    return Result(Result::Type::OK, entry->second);
  }
  if (parent != nullptr) {
    return parent->get(name);
  }
  return Result(
      Result::Type::EXCEPTION,
      new Exception("No such variable (get): '" + name + "'"));
}

Result Scope::set(const std::string &name, Reference value) {
  auto entry = bindings.find(name);
  if (entry != bindings.end()) {
    bindings[name] = value;
    return Result(Result::Type::OK, value);
  }
  if (parent != nullptr) {
    return parent->set(name, value);
  }
  return Result(
      Result::Type::EXCEPTION,
      new Exception("No such variable (set): '" + name + "'"));
}

Result Scope::declare(const std::string &name) {
  return declare(name, nil);
}

Result Scope::declare(const std::string &name, Reference value) {
  auto entry = bindings.find(name);
  if (entry == bindings.end()) {
    bindings[name] = value;
    return Result(Result::Type::OK, value);
  }
  return Result(
      Result::Type::EXCEPTION,
      new Exception("Variable already declared: '" + name + "'"));
}

Reference Scope::getClass() const {
  return classScope;
}

}