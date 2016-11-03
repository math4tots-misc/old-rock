#ifndef rock_function_hh
#define rock_function_hh

#include <string>

#include "rock/object.hh"

namespace rock {

extern Class *classFunction;

// The only two classes that inherit from 'Function' should be
// 'NativeFunction' and 'UserFunction'.
class Function: public Object {
public:
  const std::string name;
  Function(const std::string&);
  Reference getClass() const override;
  std::string debug() const override;
  virtual Result invoke(const Args&)=0;
  static Reference from(
      const std::string&, std::function<Result(const Args&)>);
};

}

#endif//rock_function_hh
