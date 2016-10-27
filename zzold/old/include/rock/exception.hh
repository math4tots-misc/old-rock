#ifndef rock_exception_hh
#define rock_exception_hh

#include <string>

#include "rock/value.hh"

namespace rock {

struct Class;

extern Class *classException;

struct Exception final: Value {
  const std::string message;
  Exception(const std::string& m): message(m) {}  // TODO: stacktrace
  Class *getClass() override { return classException; }

  std::string debugstr() const override {
    return "Exception(" + message + ")";
  }
};

}

#endif//rock_exception_hh
