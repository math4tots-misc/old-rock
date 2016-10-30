#include "rock/all.hh"

namespace rock {

Class *classNil;
Nil *nil;

namespace {
Init init(10, __FILE__, []() {
  classNil = new Class("Nil");
  builtins->declare("Nil", classNil);

  nil = new Nil();
  builtins->declare("nil", nil);
});
}

Reference Nil::getClass() const {
  return classNil;
}

}
