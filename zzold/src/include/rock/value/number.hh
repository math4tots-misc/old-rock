#ifndef rock_value_number_hh
#define rock_value_number_hh

#include <string>
#include <map>

#include "rock/value/value.hh"

namespace rock {

extern Reference<Class> classNumber;


struct Number final: Value {
  double value;
  static Reference<Number> from(double d) { return new Number(d); }
  Number(double v): value(v) {}
  Reference<Class> getClass() const override { return classNumber; };
  std::string debugstr() const override {
    return "Number(" + std::to_string(value) + ")";
  }
};

}

#endif//rock_value_number_hh
