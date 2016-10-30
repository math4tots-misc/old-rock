#ifndef rock_exception_hh
#define rock_exception_hh

#include <string>

#include "rock/object.hh"

namespace rock {

class Class;

extern Class *classException;

// TODO: Save stack trace.
class Exception final: public Object {
public:
  const std::string message;
  Exception(const std::string&);
  Reference getClass() const override;
  std::string debug() const override;
};

}

#endif//rock_exception_hh