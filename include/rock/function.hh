#ifndef rock_function_hh
#define rock_function_hh

#include "rock/object.hh"

#include <vector>
#include <string>

namespace rock {

extern Class *xFunction;

struct Function: public Object {
  const std::string name;

  Function(const std::string&);

  virtual Value callf(const Args&);
  Class *getClass() const override;
};

}  // namespace rock

#endif//rock_function_hh
