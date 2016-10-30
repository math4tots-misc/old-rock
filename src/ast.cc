#include "rock/all.hh"

namespace rock {

Ast::Ast(const Token &t): token(t) {}

Result Ast::eval(Scope&) const {
  return Result(
      Result::Type::EXCEPTION,
      new Exception(
          std::string(typeid(*this).name()) +
              " doesn't implement 'eval' yet"));
}

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

Or::Or(const Token &t, Ast *l, Ast *r): Ast(t), left(l), right(r) {}
And::And(const Token &t, Ast *l, Ast *r): Ast(t), left(l), right(r) {}
Arguments::Arguments(const Token &t, const std::vector<Ast*> &as):
    Arguments(t, as, nullptr) {}
Arguments::Arguments(const Token &t, const std::vector<Ast*> &as, Ast *va):
    token(t), args(as), vararg(va) {}

MethodCall::MethodCall(
    const Token &t, Ast *o, const std::string &n, Arguments *a):
        Ast(t), owner(o), name(n), args(a) {}

SetAttribute::SetAttribute(
    const Token &t, Ast *o, const std::string &n, Ast *v):
        Ast(t), owner(o), name(n), value(v) {}

GetAttribute::GetAttribute(
    const Token &t, Ast *o, const std::string &n):
        Ast(t), owner(o), name(n) {}

Break::Break(const Token &t): Ast(t) {}
Continue::Continue(const Token &t): Ast(t) {}

While::While(const Token &t, Ast *c, Ast *b):
    Ast(t), condition(c), body(b) {}

For::For(const Token &t, const std::string &n, Ast *c, Ast *b):
    Ast(t), name(n), container(c), body(b) {}

Return::Return(const Token &t, Ast *v): Ast(t), value(v) {}

Assignment::Assignment(const Token &t, const std::string &n, Ast *v):
    Ast(t), name(n), value(v) {}

Name::Name(const Token &t, const std::string &n):
    Ast(t), name(n) {}

Literal::Literal(const Token &t, Reference v):
    Ast(t), value(v) {}

ClassDisplay::ClassDisplay(
    const Token &t, const std::string &n, Arguments *bs, Ast *b):
        Ast(t), name(n), bases(bs), body(b) {}

Signature::Signature(
    const Token &t,
    const std::vector<std::string> &as,
    const std::vector<std::string> &oas,
    const std::string &va):
        token(t), argnames(as), optargnames(oas), varargname(va) {}

FunctionDisplay::FunctionDisplay(
    const Token &t, const std::string &n, Signature *a, Ast *b):
        Ast(t), name(n), args(a), body(b) {}

}
