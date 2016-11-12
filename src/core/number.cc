#include "rock/core/all.hh"

#include <cmath>
#include <string>

namespace rock {

Class *classNumber;

namespace {

constexpr int cachedMin = -100;
constexpr int cachedMax = 1000;

Reference cachedNumbers[cachedMax - cachedMin];

Init init(110, __FILE__, []() {
  classNumber = new Class("Number", {classObject}, {
    {"__neg", [](Reference owner, Class*, const Args& args) {
      checkargs(0, args);
      return Number::from(-owner.as<Number>()->value);
    }},
    {"__add", [](Reference owner, Class*, const Args& args) {
      checkargs(1, args);
      checktype(classNumber, args[0]);
      return Number::from(
          owner.as<Number>()->value + args[0].as<Number>()->value);
    }},
    {"__sub", [](Reference owner, Class*, const Args& args) {
      checkargs(1, args);
      checktype(classNumber, args[0]);
      return Number::from(
          owner.as<Number>()->value - args[0].as<Number>()->value);
    }},
    {"__mul", [](Reference owner, Class*, const Args& args) {
      checkargs(1, args);
      checktype(classNumber, args[0]);
      return Number::from(
          owner.as<Number>()->value * args[0].as<Number>()->value);
    }},
    {"__div", [](Reference owner, Class*, const Args& args) {
      checkargs(1, args);
      checktype(classNumber, args[0]);
      return Number::from(
          owner.as<Number>()->value / args[0].as<Number>()->value);
    }},
    {"__mod", [](Reference owner, Class*, const Args& args) {
      checkargs(1, args);
      checktype(classNumber, args[0]);
      return Number::from(std::fmod(
          owner.as<Number>()->value, args[0].as<Number>()->value));
    }},
    {"__lt", [](Reference owner, Class*, const Args& args) {
      checkargs(1, args);
      checktype(classNumber, args[0]);
      return Bool::from(
          owner.as<Number>()->value < args[0].as<Number>()->value);
    }},
    {"__eq", [](Reference owner, Class*, const Args& args) -> Reference {
      checkargs(1, args);
      if (!instanceof(args[0], classNumber)) {
        return xfalse;
      }
      return Bool::from(
          owner.as<Number>()->value == args[0].as<Number>()->value);
    }},
    {"isCloseTo", [](Reference owner, Class*, const Args& args) {
      checkargs(1, args);
      checktype(classNumber, args[0]);
      // Modeled after Python's math.isclose function.
      double a = owner.as<Number>()->value;
      double b = args[0].as<Number>()->value;
      return Bool::from(
          std::abs(a - b) <= 1e-09 * std::max(std::abs(a), std::abs(b)));
    }},
  });
  builtins->declare("Number", classNumber);
  for (int i = cachedMin; i < cachedMax; i++) {
    cachedNumbers[i - cachedMin] = new Number(i);
  }
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
  if (value >= cachedMin && value < cachedMax && std::floor(value) == value) {
    return cachedNumbers[static_cast<int>(value) - cachedMin];
  }
  return new Number(value);
}

}
