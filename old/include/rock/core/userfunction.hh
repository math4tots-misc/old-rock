#ifndef rock_core_userfunction_hh
#define rock_core_userfunction_hh

#include <string>

#include "rock/core/function.hh"

namespace rock {

class Scope;

class UserFunction final: public Function {
public:
  const FunctionDisplay *const node;
  const Reference parentScope;
  UserFunction(const std::string&, const FunctionDisplay*, Scope&);
  Reference invoke(const Args&) override;
  std::string debug() const override;
};

}

#endif//rock_core_userfunction_hh
