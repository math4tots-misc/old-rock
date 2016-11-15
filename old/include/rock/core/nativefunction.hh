#ifndef rock_core_nativefunction_hh
#define rock_core_nativefunction_hh

#include <string>

#include "rock/core/function.hh"

namespace rock {

class NativeFunction final: public Function {
public:
  std::function<Reference(const Args&)> function;
  NativeFunction(const std::string&, std::function<Reference(const Args&)>);
  std::string debug() const override;
  Reference invoke(const Args&) override;
};

}

#endif//rock_core_nativefunction_hh
