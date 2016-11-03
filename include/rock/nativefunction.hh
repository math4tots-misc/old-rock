#ifndef rock_nativefunction_hh
#define rock_nativefunction_hh

#include <string>

#include "rock/function.hh"

namespace rock {

class NativeFunction final: public Function {
public:
  std::function<Result(const Args&)> function;
  NativeFunction(const std::string&, std::function<Result(const Args&)>);
  std::string debug() const override;
  Result invoke(const Args&) override;
};

}

#endif//rock_nativefunction_hh
