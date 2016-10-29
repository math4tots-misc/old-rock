#ifndef rock_ast_ast_hh
#define rock_ast_ast_hh

#include <string>

#include "rock/ast/file.hh"
#include "rock/base/result.hh"
#include "rock/value/nil.hh"
#include "rock/value/scope.hh"
#include "rock/value/value.hh"

namespace rock {

struct Ast {
  Token *const token;
  Ast(Token *t): token(t) {}
  virtual Result<Ref> eval(Scope *scope) const=0;
  virtual bool isError() const { return false; }
};

struct ParseError final: Ast {
  const std::string message;
  ParseError(Token *t, const std::string& m): Ast(t), message(m) {}
  Result<Ref> eval(Scope *scope) const override {
    return Result<Ref>(EXCEPTION, nil);
  }
  bool isError() const override { return true; }
};

struct Module final: Ast {
  const std::vector<Ast*> expressions;
  Module(Token *t, const std::vector<Ast*>& es):
      Ast(t), expressions(es) {}
  Result<Ref> eval(Scope *scope) const {
    Ref last(nil);
    for (Ast *e: expressions) {
      Result<Ref> result = e->eval(scope);
      if (result.type != NORMAL) {
        return result;
      }
      last = result.value;
    }
    return Result<Ref>(NORMAL, last);
  }
};

struct Block final: Ast {
  const std::vector<Ast*> expressions;
  Block(Token *t, const std::vector<Ast*>& es):
      Ast(t), expressions(es) {}
  Result<Ref> eval(Scope *parentScope) const {
    Scope scope(parentScope);
    Ref last(nil);
    for (Ast *e: expressions) {
      Result<Ref> result = e->eval(&scope);
      if (result.type != NORMAL) {
        return result;
      }
      last = result.value;
    }
    return Result<Ref>(NORMAL, last);
  }
};

struct Literal final: Ast {
  Ref value;
  Literal(Token *t, Ref v): Ast(t), value(v) {}
  Result<Ref> eval(Scope *scope) const {
    return Result<Ref>(NORMAL, value);
  }
};

struct Name final: Ast {
  const std::string name;
  Name(Token *t, const std::string& n): Ast(t), name(n) {}
  Result<Ref> eval(Scope *scope) const { return scope->get(name); }
};

struct Declare final: Ast {
  const std::string name;
  Ast *const value;
  Declare(Token *t, Ast *const v): Ast(t), value(v) {}
  Result<Ref> eval(Scope *scope) const {
    Result<Ref> r = value->eval(scope);
    return r.type == NORMAL ? scope->declare(name, r.value) : r;
  }
};

struct Assign final: Ast {
  const std::string name;
  Ast *const value;
  Assign(Token *t, Ast *const v): Ast(t), value(v) {}
  Result<Ref> eval(Scope *scope) const {
    Result<Ref> r = value->eval(scope);
    return r.type == NORMAL ? scope->set(name, r.value) : r;
  }
};

struct MethodCall final: Ast {
  Ast *owner;
  const std::string name;
  std::vector<Ast*> args;
  MethodCall(
      Token *t, Ast *onr, const std::string& n, const std::vector<Ast*>& aa):
      Ast(t), owner(onr), name(n), args(aa) {}
  Result<Ref> eval(Scope *scope) const {
    std::vector<Ref> args;
    Result<Ref> ownerResult = this->owner->eval(scope);
    if (ownerResult.type != NORMAL) { return ownerResult; }
    Ref owner = ownerResult.value;
    for (Ast *arg: this->args) {
      Result<Ref> r = arg->eval(scope);
      if (r.type != NORMAL) { return r; }
      args.push_back(r.value);
    }
    return owner->call(name, args);
  }
};

}

#endif//rock_ast_ast_hh
