#ifndef rock_string_hh
#define rock_string_hh

#include <string>

#include "rock/value.hh"

namespace rock {

struct Class;

extern Class *classString;

struct String final: Value {
  const std::string value;
  String(const std::string& s): value(s) {}
  Class *getClass() override { return classString; }
};

}

#endif//rock_string_hh
