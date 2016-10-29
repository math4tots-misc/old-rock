#ifndef rock_value_string_hh
#define rock_value_string_hh

#include <string>
#include <map>

#include "rock/value/value.hh"
#include "rock/value/class.hh"

namespace rock {

extern Reference<Class> classString;


struct String final: Value {
  std::string value;
  String(const std::string& v): value(v) {}
  Reference<Class> getClass() const override { return classString; };
  std::string debugstr() const override {
    return "String(" + value + ")";
  }
};

}

#endif//rock_value_string_hh
