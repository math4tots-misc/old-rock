#include "rock/all.hh"

namespace rock {

Class *classNumber;

namespace {
Init init(10, __FILE__, []() {
  classNumber = new Class("Number");
  builtins->declare("Number", classNumber);
});
}

Number::Number(double v): value(v) {}
Reference Number::getClass() const {
  return classNumber;
}

}
