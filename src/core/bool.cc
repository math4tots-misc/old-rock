#include "rock/core/all.hh"

namespace rock {

Class *classBool;
Bool *xtrue;
Bool *xfalse;

namespace {
Init init(110, __FILE__, []() {
  classBool = new Class("Bool", {classObject}, {
    {"__not", [](Reference owner, const Args &args) {
      checkargs(0, args);
      return Bool::from(!owner->truthy());
    }},
  });
  builtins->declare("Bool", classBool);

  xtrue = new Bool(true);
  builtins->declare("true", xtrue);
  xfalse = new Bool(false);
  builtins->declare("false", xfalse);
});
}

Bool::Bool(bool v): value(v) {}

Reference Bool::getClass() const {
  return classBool;
}

std::string Bool::debug() const {
  return value ? "Bool(true)" : "Bool(false)";
}

bool Bool::truthy() const {
  return value;
}

std::string Bool::str() const {
  return value ? "true" : "false";
}

Reference Bool::from(bool value) {
  return Reference(value ? xtrue : xfalse);
}
}
