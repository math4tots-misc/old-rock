#include "rock/all.hh"

namespace rock {

Ast::Ast(const Token &t): token(t) {}

ParseError::ParseError(const Token &t, const std::string &m):
    Ast(t), message(m) {}

Result ParseError::eval(Scope& scope) {
  // TODO: Save token on stack, so that the location of the parse
  // error shows up on stack trace.
  return Result(
      Result::Type::EXCEPTION,
      new Exception("Parse error: " + message));
}

}
