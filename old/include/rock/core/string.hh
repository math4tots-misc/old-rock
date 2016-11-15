#ifndef rock_core_string_hh
#define rock_core_string_hh

#include <string>

#include "rock/core/object.hh"

namespace rock {

class Class;

extern Class *classString;

class String final: public Object {
public:
  const std::string value;
  String(const std::string&);
  Reference getClass() const override;
  std::string debug() const override;
  std::string str() const override;
  static Reference from(const std::string&);
};

}

#endif//rock_core_string_hh
