#ifndef rock_nil_hh
#define rock_nil_hh

#include <string>

#include "rock/value.hh"

namespace rock {

struct Class;

extern Class *classNil;

struct Nil final: Value {
  Class *getClass() override { return classNil; }
};

extern Nil *nil;

}

#endif//rock_nil_hh
