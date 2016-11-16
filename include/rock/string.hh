#ifndef rock_string_hh
#define rock_string_hh

#include "rock/object.hh"

#include <vector>
#include <string>

namespace rock {

extern Class *xString;

struct String final: public Object {
  const std::string value;

  String(const std::string&);

  Class *getClass() const override;
  void traverse(void*, void(*)(void*,Object*)) const override;
};

}  // namespace rock

#endif//rock_string_hh
