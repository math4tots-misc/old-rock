#include "rock/all.hh"

namespace rock {

Class *classString;

namespace {
Init init(10, __FILE__, []() {
  classString = new Class("String");
  acquire(classString);  // live forever
});
}

String::String(const std::string &v): value(v) {}
Reference String::getClass() const {
  return classString;
}

}
