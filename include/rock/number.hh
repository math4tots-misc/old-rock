#ifndef rock_number_hh
#define rock_number_hh

#include <string>

#include "rock/object.hh"

namespace rock {

class Class;

extern Class *classNumber;

// TODO: Save stack trace.
class Number final: public Object {
public:
  const double value;
  Number(double);
  Reference getClass() const override;
};

}

#endif//rock_number_hh
