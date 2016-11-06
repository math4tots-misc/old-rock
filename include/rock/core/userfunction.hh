#ifndef rock_userfunction_hh
#define rock_userfunction_hh

#include <string>

#include "rock/core/function.hh"

namespace rock {

class Scope;

class UserFunction final: public Function {
public:
  const FunctionDisplay *const node;
  const Reference parentScope;
  UserFunction(const std::string&, const FunctionDisplay*, Scope&);
  Result invoke(const Args&) override;
};

}

#endif//rock_userfunction_hh
