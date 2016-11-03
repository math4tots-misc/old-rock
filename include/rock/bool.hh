#ifndef rock_bool_hh
#define rock_bool_hh

#include <string>

#include "rock/object.hh"

namespace rock {

class Class;
class Bool;

extern Class *classBool;
extern Bool *xtrue;
extern Bool *xfalse;

class Bool final: public Object {
public:
  const bool value;
  Bool(bool);
  Reference getClass() const override;
  std::string debug() const override;
  bool truthy() const override;
  std::string str() const override;
  static Reference from(bool);
};

}

#endif//rock_bool_hh
