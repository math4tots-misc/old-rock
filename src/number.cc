#include "rock/all.hh"

#include <cmath>
#include <string>

namespace rock {

Class *classNumber;

namespace {
Init init(110, __FILE__, []() {
  classNumber = new Class("Number", {classObject}, {
    {"__add", [](Reference owner, const Args& args) {
      checkargs(1, args);
      checktype(classNumber, args[0]);
      return Result(
          Result::Type::OK,
          Number::from(
              owner.as<Number>()->value +
              args[0].as<Number>()->value));
    }},
    {"__lt", [](Reference owner, const Args& args) {
      checkargs(1, args);
      checktype(classNumber, args[0]);
      return Result(
          Result::Type::OK,
          Bool::from(
              owner.as<Number>()->value <
              args[0].as<Number>()->value));
    }},
    {"__eq", [](Reference owner, const Args& args) {
      checkargs(1, args);
      if (!instanceof(args[0], classNumber)) {
        return Result(Result::Type::OK, xfalse);
      }
      return Result(
          Result::Type::OK,
          Bool::from(
              owner.as<Number>()->value ==
              args[0].as<Number>()->value));
    }},
  });
  builtins->declare("Number", classNumber);
});
}

Number::Number(double v): value(v) {}

Reference Number::getClass() const {
  return classNumber;
}

std::string Number::str() const {
  double integer_part;
  return
      std::modf(value, &integer_part) == 0 ?
      std::to_string((long) value) :
      std::to_string(value);
}

Reference Number::from(double value) {
  // TODO: Do something more efficient
  return new Number(value);
}

}
