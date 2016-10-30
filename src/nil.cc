#include "rock/all.hh"

namespace rock {

Class *classNil;
Nil *nil;

namespace {
Init init(10, __FILE__, []() {
  classNil = new Class("Nil");
  acquire(classNil);  // live forever

  nil = new Nil();
  acquire(nil);  // live forever
});
}

Reference Nil::getClass() const {
  return classNil;
}

}
