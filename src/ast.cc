#include "rock/all.hh"

namespace rock {

Ast::Ast(const Token &t): token(t) {}

std::string Ast::debug() const {
  return typeid(*this).name();
}

ParseError::ParseError(const Token &t, const std::string &m):
    Ast(t), message(m) {}

Result ParseError::eval(Scope &scope) const {
  // TODO: Save token on stack, so that the location of the parse
  // error shows up on stack trace.
  return Result(
      Result::Type::EXCEPTION,
      new Exception("Parse error: " + message));
}

std::string ParseError::debug() const {
  return "ParseError(" + message + ")";
}

Block::Block(const Token &t, const std::vector<Ast*> &es):
    Ast(t), expressions(es) {}

Result Block::eval(Scope &scope) const {
  Result last = Result(Result::Type::OK, nil);
  for (Ast *e: expressions) {
    last = e->eval(scope);
    if (last.type != Result::Type::OK) { return last; }
  }
  return last;
}

If::If(const Token &t, Ast *c, Ast *b, Ast *o):
    Ast(t), condition(c), body(b), other(o) {}

Result If::eval(Scope &scope) const {
  Result c = condition->eval(scope);
  if (c.type != Result::Type::OK) { return c; }

  if (c.value->truthy()) {
    return body->eval(scope);
  } else if (other) {
    return other->eval(scope);
  }
  return Result(Result::Type::OK, nil);
}

}
