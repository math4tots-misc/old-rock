#ifndef rock_ast_hh
#define rock_ast_hh

#include <vector>

#include "rock/file.hh"
#include "rock/scope.hh"
#include "rock/value/class.hh"
#include "rock/value/string.hh"
#include "rock/value/value.hh"

namespace rock {

constexpr long NORMAL = 1;
constexpr long RETURN = 2;
constexpr long BREAK = 3;
constexpr long CONTINUE = 4;
constexpr long EXCEPTION = 5;

struct Result {
  const long type;
  const P value;

  Result(long t): Result(t, P()) {}
  Result(long t, P p): type(t), value(p) {}
};

struct Ast {
  const Token token;

  Ast(const Token& t): token(t) {}
  virtual Result eval(Scope& scope)=0;
};

struct Block final: Ast {
  const std::vector<Ast*> exprs;

  Block(const Token& t, const std::vector<Ast*>& es): Ast(t), exprs(es) {}
  Result eval(Scope& scope) override {
    P last;
    for (auto expr: exprs) {
      Result result = expr->eval(scope);
      if (result.type != NORMAL) {
        return result;
      }
      last = result.value;
    }
    return Result(NORMAL, last);
  }
};

struct Literal final: Ast {
  P value;
  Literal(const Token& t, const std::string& s):
      Ast(t), value(new String(s)) {}
  Literal(const Token& t, const Nil& n):
      Ast(t), value() {}
  Result eval(Scope& scope) override {
    return Result(NORMAL, value);
  }
};

struct Name final: Ast {
  std::string name;
  Name(const Token& t, const std::string& n): Ast(t), name(n) {}
  Result eval(Scope& scope) override {
    if (!scope.has(name)) {
      // TODO: Don't use string as exception
      return Result(
        EXCEPTION, String::from("No such variable: " + name));
    }
    return Result(NORMAL, scope.get(name));
  }
};

}

#endif//rock_ast_hh
