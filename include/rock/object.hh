#ifndef rock_object_hh
#define rock_object_hh

#include "rock/value.hh"

#include <vector>
#include <string>
#include <map>
#include <type_traits>

namespace rock {

extern Class *xObject;

struct Object {
  virtual Class *getClass() const=0;
  virtual void traverse(void*, void(*)(void*,Object*)) const=0;
  Value call(Class*, const std::string&, const Args&);
};

}  // namespace rock

#endif//rock_object_hh
