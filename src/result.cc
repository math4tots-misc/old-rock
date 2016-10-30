#include "rock/all.hh"

#include <sstream>

namespace rock {

Result::Result(Reference r): Result(Result::Type::OK, r) {}
Result::Result(Type t, Reference r): type(t), value(r) {}
Reference Result::get() const {
  if (type != Result::Type::OK) {
    // TODO: Use a real error class.
    throw Reference(new Exception(
        "Result type is not OK: " + std::to_string(type)));
  }
  return value;
}

std::string Result::debug() const {
  std::string message = "Result(" + std::to_string(type);
  if (value.operator->()) {
    message += "," + value->debug();
  }
  message += ")";
  return message;
}

}  // namespace rock

namespace std {

std::string to_string(rock::Result::Type t) {
  switch(t) {
  case rock::Result::Type::OK: return "OK";
  case rock::Result::Type::EXCEPTION: return "EXCEPTION";
  case rock::Result::Type::RETURN: return "RETURN";
  case rock::Result::Type::BREAK: return "BREAK";
  case rock::Result::Type::CONTINUE: return "CONTINUE";
  }
}

}  // namespace std
