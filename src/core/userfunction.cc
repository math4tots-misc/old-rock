#include "rock/core/all.hh"

namespace rock {

UserFunction::UserFunction(
    const std::string &n,
    const FunctionDisplay *f,
    Scope &s): Function(n), node(f), parentScope(&s) {}

Result UserFunction::invoke(const Args &args) {
  Reference scope(new Scope(parentScope.as<Scope>()));
  node->args->resolve(*scope.as<Scope>(), args);
  Result result = node->body->eval(*scope.as<Scope>());
  switch (result.type) {
  case Result::Type::OK:
  case Result::Type::EXCEPTION:
    return result;
  case Result::Type::RETURN:
    return Result(Result::Type::OK, result.value);
  default:
    // TODO: Do something nicer
    return Result(
        Result::Type::EXCEPTION, new Exception("Invalid result type"));
  }
}

std::string UserFunction::debug() const {
  return
    "<UserFunction " + name + "@" +
    node->token.compactLocationMessage() + ">";
}

}
