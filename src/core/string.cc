#include "rock/core/all.hh"

namespace rock {

Class *classString;

namespace {
Init init(110, __FILE__, []() {
  classString = new Class("String", {classObject}, {
    {"__str", [](Reference owner, Class*, const Args &args) {
      checkargs(0, args);
      return owner;
    }},
    {"__eq", [](Reference owner, Class*, const Args& args) -> Reference {
      checkargs(1, args);
      if (!instanceof(args[0], classString)) {
        return xfalse;
      }
      return Bool::from(
          owner.as<String>()->value == args[0].as<String>()->value);
    }},
    {"__add", [](Reference owner, Class*, const Args& args) -> Reference {
      checkargs(1, args);
      checktype(classString, args[0]);
      return String::from(
          owner.as<String>()->value + args[0].as<String>()->value);
    }},
  });
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

Reference String::from(const std::string &value) {
  return new String(value);
}

}
