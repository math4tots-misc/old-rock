#include "rock/all.hh"

namespace rock {

Ast::Ast(const Token &t): token(t) {}

ParseError::ParseError(const Token &t, const std::string &m):
    Ast(t), message(m) {}

Result ParseError::eval(Scope& scope) {
  // TODO: return Result(EXCEPTION, ...)
  throw "Parse error";
}

}
