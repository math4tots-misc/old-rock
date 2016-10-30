#ifndef rock_nil_hh
#define rock_nil_hh

#include <string>

#include "rock/object.hh"

namespace rock {

class Class;
class Nil;

extern Class *classNil;
extern Nil *nil;

class Nil final: public Object {
public:
  Reference getClass() const override;
};

}

#endif//rock_nil_hh
