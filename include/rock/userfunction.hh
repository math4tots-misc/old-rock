#ifndef rock_userfunction_hh
#define rock_userfunction_hh

#include "rock/function.hh"

#include <vector>
#include <string>

namespace rock {

struct Ast;

struct UserFunction final: public Function {
  std::string name;
  Object *scope;
  Ast *node;

  UserFunction(const std::string&, Object*, Ast*);
  Class *getClass() const override;
  void traverse(void*, void(*)(void*,Object*)) const override;
};

}  // namespace rock

#endif//rock_userfunction_hh
