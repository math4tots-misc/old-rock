#ifndef rock_ast_hh
#define rock_ast_hh

#include <vector>

#include "rock/file.hh"
#include "rock/scope.hh"
#include "rock/value.hh"

namespace rock {

struct Ast {
  const Token token;

  Ast(const Token& t): token(t) {}
  virtual P eval(Scope& scope)=0;
};

struct Block final: Ast {
  const std::vector<Ast*> exprs;

  Block(const Token& t, const std::vector<Ast*>& es): Ast(t), exprs(es) {}
  P eval(Scope& scope) override {
    P last;
    for (auto expr: exprs) {
      last = expr->eval(scope);
    }
    return last;
  }
};

}

#endif//rock_ast_hh
