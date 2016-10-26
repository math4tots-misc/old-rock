#ifndef rock_ast_hh
#define rock_ast_hh

#include <string>

#include "rock/result.hh"
#include "rock/reference.hh"
#include "rock/nil.hh"
#include "rock/Scope.hh"

namespace rock {

struct Token;
struct Reference;
struct Result;

struct Ast {
  Token *const token;
  Ast(Token *t): token(t) {}
  virtual Result eval(Scope *scope) const=0;
  virtual bool isError() const { return false; }
};

struct ParseError final: Ast {
  const std::string message;
  ParseError(Token *t, const std::string& m): Ast(t), message(m) {}
  Result eval(Scope *scope) const override { return Result(EXCEPTION, nil); }
  bool isError() const override { return true; }
};

struct Module final: Ast {
  const std::vector<Ast*> expressions;
  Module(Token *t, const std::vector<Ast*>& es):
      Ast(t), expressions(es) {}
  Result eval(Scope *scope) const {
    for (Ast *e: expressions) {
      Result result = e->eval(scope);
      if (result.type != NORMAL) {
        return result;
      }
    }
    return Result(NORMAL, nil);
  }
};

struct Block final: Ast {
  const std::vector<Ast*> expressions;
  Block(Token *t, const std::vector<Ast*>& es):
      Ast(t), expressions(es) {}
  Result eval(Scope *parentScope) const {
    Scope scope(parentScope);
    Reference last(nil);
    for (Ast *e: expressions) {
      Result result = e->eval(&scope);
      if (result.type != NORMAL) {
        return result;
      }
      last = result.value;
    }
    return Result(NORMAL, last);
  }
};

struct Literal final: Ast {
  Reference value;
  Literal(Token *t, Reference v): Ast(t), value(v) {}
  Result eval(Scope *scope) const {
    return Result(NORMAL, value);
  }
};

struct Name final: Ast {
  const std::string name;
  Name(Token *t, const std::string& n): Ast(t), name(n) {}
  Result eval(Scope *scope) const { return scope->get(name); }
};

struct Declare final: Ast {
  const std::string name;
  Ast *const value;
  Declare(Token *t, Ast *const v): Ast(t), value(v) {}
  Result eval(Scope *scope) const {
    Result r = value->eval(scope);
    return r.type == NORMAL ? scope->declare(name, r.value) : r;
  }
};

struct Assign final: Ast {
  const std::string name;
  Ast *const value;
  Assign(Token *t, Ast *const v): Ast(t), value(v) {}
  Result eval(Scope *scope) const {
    Result r = value->eval(scope);
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
  Result eval(Scope *scope) const {
    std::vector<Reference> args;
    Result ownerResult = this->owner->eval(scope);
    if (ownerResult.type != NORMAL) { return ownerResult; }
    Reference owner = ownerResult.value;
    for (Ast *arg: this->args) {
      Result r = arg->eval(scope);
      if (r.type != NORMAL) { return r; }
      args.push_back(r.value);
    }
    return owner.call(name, args);
  }
};

}

#endif//rock_ast_hh
