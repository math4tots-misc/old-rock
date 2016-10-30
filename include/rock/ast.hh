#ifndef rock_ast_hh
#define rock_ast_hh

#include "rock/file.hh"
#include "rock/object.hh"

namespace rock {

class Result;
class Scope;

class Ast {
public:
  const Token token;
  Ast(const Token& t);
  virtual Result eval(Scope)=0;
};

}

#endif//rock_ast_hh
