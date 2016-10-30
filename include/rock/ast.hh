#ifndef rock_ast_hh
#define rock_ast_hh

#include <memory>

#include "rock/file.hh"
#include "rock/object.hh"

namespace rock {

class Result;
class Scope;

class Ast {
public:
  const Token token;
  Ast(const Token& t);
  virtual Result eval(Scope&)=0;
};

class ParseError final: public Ast {
public:
  const std::string message;
  ParseError(const Token&, const std::string&);
  Result eval(Scope&);
};

class Block final: public Ast {
public:
  const std::vector<std::unique_ptr<Ast>> expressions;
  Block(const std::vector<std::unique_ptr<Ast>>&);
  Result eval(Scope&);
};

class If final: public Ast {
public:
  const std::unique_ptr<Ast> condition, body, other;
  If(std::unique_ptr<Ast> c, std::unique_ptr<Ast> b, std::unique_ptr<Ast> o);
  Result eval(Scope&);
};

class Signature final {
  const Token token;
  const std::vector<std::string> argnames;
  const std::vector<std::string> optargnames;
  const std::string varargname;
};

class Arguments final {
public:
  const Token token;
  std::vector<std::unique_ptr<Ast>> args;
  std::unique_ptr<Ast> vararg; // TODO: support varargs and iterables.
  Arguments(const Token&, const std::vector<std::unique_ptr<Ast>>&);
  Arguments(
      const Token&,
      const std::vector<std::unique_ptr<Ast>>&,
      std::unique_ptr<Ast>);
  Result evalargs(Scope&, std::vector<Reference>&);
};

}

#endif//rock_ast_hh
