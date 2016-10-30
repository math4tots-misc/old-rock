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
  virtual Result eval(Scope&);
};

}

#endif//rock_ast_hh
