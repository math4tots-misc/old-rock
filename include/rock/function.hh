#ifndef rock_function_hh
#define rock_function_hh

#include <string>

#include "rock/object.hh"

namespace rock {

extern Class *classFunction;

class Function: public Object {
public:
  const std::string name;
  std::function<Result(const Args&)> function;
  Function(const std::string&, std::function<Result(const Args&)>);
  Reference getClass() const override;
  std::string debug() const override;
};

}

#endif//rock_function_hh
