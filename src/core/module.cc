#include "rock/core/all.hh"

#include <set>
#include <map>

namespace rock {

std::map<std::string,Reference> *moduleRegistry;


namespace {
Init init(110, __FILE__, []() {
  moduleRegistry = new std::map<std::string,Reference>();
});
}

Reference moduleFrom(const std::string &name, Block *block) {
  Reference scope(new Scope(builtins));
  scope.as<Scope>()->declare("__name", String::from(name));
  Result result = block->evalWithoutNewScope(*scope.as<Scope>());
  if (result.type != Result::Type::OK) { throw result; }
  std::map<std::string,Reference> bindings = scope.as<Scope>()->getBindings();
  std::map<std::string,Method> methods;
  for (auto it = bindings.begin(); it != bindings.end(); ++it) {
    std::string key = it->first;
    Reference value = it->second;
    methods["__get_" + key] = [=](Reference, const Args &args) {
      checkargs(0, args);
      return value;
    };
    methods[key] = [=](Reference, const Args &args) {
      return value->call("__call", args);
    };
  }
  return new UserObject(new Class(
      "__ModuleClass", {classObject}, true, {}, methods));
}

}
