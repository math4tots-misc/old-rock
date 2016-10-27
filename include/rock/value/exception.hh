#ifndef rock_value_exception_hh
#define rock_value_exception_hh

#include <string>
#include <map>

#include "rock/value/value.hh"
#include "rock/value/class.hh"

namespace rock {

extern Reference<Class> classException;


struct Exception final: Value {
  const std::string message;
  Exception(const std::string& m): message(m) {}
  Reference<Class> getClass() const override { return classException; };
  std::string debugstr() const override {
    return "Exception(" + message + ")";
  }
};

}

#endif//rock_value_exception_hh
