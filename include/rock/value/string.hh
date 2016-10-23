#ifndef rock_value_string_hh
#define rock_value_string_hh

#include "rock/value/class.hh"

namespace rock {

Class classString("String");  // TODO: cc

struct String final: Value {
  const std::string value;

  static P from(const std::string& s) { return P(new String(s)); }

  String(const std::string& v) : value(v) {}
  Class& getClass() const override { return classString; }
  size_t hash() override { return std::hash<std::string>()(value); }
};

}

#endif//rock_value_string_hh
