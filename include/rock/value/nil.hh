#ifndef rock_value_nil_hh
#define rock_value_nil_hh

#include <string>
#include <map>

#include "rock/value/value.hh"

namespace rock {

extern Reference<Class> classNil;


struct Nil final: Value {
  Reference<Class> getClass() const override { return classNil; };
  std::string debugstr() const override { return "nil"; }
};

extern Reference<Nil> nil;

}

#endif//rock_value_nil_hh
