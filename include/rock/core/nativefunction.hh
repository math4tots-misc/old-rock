#ifndef rock_core_nativefunction_hh
#define rock_core_nativefunction_hh

#include <string>

#include "rock/core/function.hh"

namespace rock {

class NativeFunction final: public Function {
public:
  Reference(*function)(const Args&);
  NativeFunction(const std::string&, Reference(*)(const Args&));
  std::string debug() const override;
  Reference invoke(const Args&) override;
};

}

#endif//rock_core_nativefunction_hh
