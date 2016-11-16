#ifndef rock_nativefunction_hh
#define rock_nativefunction_hh

#include "rock/function.hh"

#include <vector>
#include <string>

namespace rock {

struct NativeFunction final: public Function {
  std::string name;
  Value (*impl)(Value, Class*, const Args&);

  Value callf(const Args&) override;
  void traverse(void*, void(*)(void*,Object*)) const override;
};

}  // namespace rock

#endif//rock_nativefunction_hh
