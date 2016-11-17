#ifndef rock_core_method_hh
#define rock_core_method_hh

#include <vector>
#include <iostream>

namespace rock {

class Reference;
class Class;

using Args = std::vector<Reference>;

class Method final {
  using Implementation = Reference(*)(void*,Reference,Class*,const Args&);

  void *data;
  Implementation impl;

public:
  Method() = default;

  template <class T>
  Method(T functor);

  // Method(Reference(*)(Reference,Class*,const Args&));
  //
  // template <>
  // Method(Implementation);
  //
  Reference operator()(Reference r, Class *c, const Args &as);
  explicit operator bool() const;
};

}  // namespace rock

#endif//rock_core_method_hh
