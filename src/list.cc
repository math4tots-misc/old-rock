#include "rock/all.hh"

#include <sstream>

namespace rock {

Class *classList;

namespace {
Init init(110, __FILE__, []() {
  classList = new Class("List", {classObject}, {
    {"__str", [](Reference owner, const Args& args) {
      if (args.size() != 0) {
        return Result(
            Result::Type::EXCEPTION, new Exception("Expected 0 arguments"));
      }
      return Result(Result::Type::OK, new String(owner->str()));
    }}
  });
  builtins->declare("List", classList);
});
}

List::List(const std::vector<Reference>& v): value(v) {}

Reference List::getClass() const {
  return classList;
}

std::string List::debug() const {
  std::stringstream ss;
  ss << "L(";
  bool first = true;
  for (const Reference& r: value) {
    if (!first) {
      ss << ", ";
    }
    ss << r->debug();
    first = false;
  }
  ss << ")";
  return ss.str();
}

std::string List::str() const {
  std::stringstream ss;
  ss << "L(";
  bool first = true;
  for (const Reference& r: value) {
    if (!first) {
      ss << ", ";
    }
    ss << r->str();
    first = false;
  }
  ss << ")";
  return ss.str();
}

}
