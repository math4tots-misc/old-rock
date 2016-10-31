#include "rock/all.hh"

namespace rock {

Class *classString;

namespace {
Init init(110, __FILE__, []() {
  classString = new Class("String");
  builtins->declare("String", classString);
});
}  // namespace

String::String(const std::string &v): value(v) {}

Reference String::getClass() const {
  return classString;
}

std::string String::debug() const {
  return "String(" + value + ")";
}

std::string String::str() const {
  return value;
}

}
