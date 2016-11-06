#ifndef rock_core_number_hh
#define rock_core_number_hh

#include <string>

#include "rock/core/object.hh"

namespace rock {

class Class;

extern Class *classNumber;

class Number final: public Object {
public:
  const double value;
  Number(double);
  Reference getClass() const override;
  std::string str() const override;

  static Reference from(double);
};

}

#endif//rock_core_number_hh
