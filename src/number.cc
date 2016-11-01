#include "rock/all.hh"

#include <cmath>
#include <string>

namespace rock {

Class *classNumber;

namespace {
Init init(110, __FILE__, []() {
  classNumber = new Class("Number", {classObject}, {
    {"__add", [](Reference owner, const Args& args) {
      if (args.size() != 1) {
        return Result(
            Result::Type::EXCEPTION,
            new Exception(
                "Expected 1 arguments but got " +
                std::to_string(args.size())));
      }
      Number *n = dynamic_cast<Number*>(args[0].operator->());
      if (!n) {
        return Result(
            Result::Type::EXCEPTION,
            new Exception(
                "Expected Number but got " +
                args[0]->getClass().as<Class>()->name));
      }
      return Result(
          Result::Type::OK,
          new Number(owner.as<Number>()->value + n->value));
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

}
