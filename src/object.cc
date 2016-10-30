#include "rock/all.hh"

#include <sstream>

namespace rock {

Result Object::call(const std::string &name, const Args &args) {
  Reference clsref = getClass();
  Class *cls = clsref.as<Class>();
  Method method = cls->getMethod(name);
  if (!method) {
    const std::string message =
        "No such method: " + name + " for class " + cls->name;
    return Result(
        Result::Type::EXCEPTION,
        new Exception(message));
  }
  return method(this, args);
}

std::string Object::debug() const {
  std::stringstream ss;
  ss << "<Object("
      << std::string(typeid(*this).name())
      << "," << this << ")>";
  return ss.str();
}

bool Object::truthy() const {
  return true;
}

std::string Object::str() const {
  return debug();
}

}  // namespace rock
