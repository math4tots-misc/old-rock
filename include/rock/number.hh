#ifndef rock_number_hh
#define rock_number_hh

#include <string>

#include "rock/value.hh"

namespace rock {

struct Class;

extern Class *classNumber;

struct Number final: Value {
  double value;
  Number(double s): value(s) {}
  Class *getClass() override { return classNumber; }
};

}

#endif//rock_number_hh
