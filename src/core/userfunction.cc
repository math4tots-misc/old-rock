#include "rock/core/all.hh"

namespace rock {

UserFunction::UserFunction(
    const std::string &n,
    const FunctionDisplay *f,
    Scope &s): Function(n), node(f), parentScope(&s) {}

Reference UserFunction::invoke(const Args &args) {
  Reference scope(new Scope(parentScope.as<Scope>()));
  node->args->resolve(*scope.as<Scope>(), args);
  return node->body->eval(*scope.as<Scope>());
}

std::string UserFunction::debug() const {
  return
    "<UserFunction " + name + "@" +
    node->token.compactLocationMessage() + ">";
}

}
