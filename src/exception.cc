#include "rock/all.hh"

namespace rock {

Class *classException;

namespace {
Init init(10, __FILE__, []() {
  classException = new Class("Exception");
  acquire(classException);  // live forever
});
}

Exception::Exception(const std::string &m): message(m) {}

Reference Exception::getClass() const {
  return classException;
}

std::string Exception::debug() const {
  return "<Exception: " + message + ">";
}

}
