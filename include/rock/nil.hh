#ifndef rock_nil_hh
#define rock_nil_hh

#include "rock/object.hh"

#include <vector>
#include <string>
#include <map>
#include <type_traits>

namespace rock {

extern Class *xNil;

struct Nil final: public Object {
  Class *getClass() const;
  void traverse(void*, void(*)(void*,Object*)) const;
};

}  // namespace rock

#endif//rock_nil_hh
