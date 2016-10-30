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

void Reference::acquire(Object *pointer) {
  if (pointer) {
    pointer->refcnt++;
  }
}

void Reference::release(Object *pointer) {
  if (pointer) {
    pointer->refcnt--;
    if (pointer->refcnt <= 0) {
      delete pointer;
      pointer = nullptr;
    }
  }
}

Reference::Reference(): pointer(nullptr) {}
Reference::Reference(Object *p): pointer(p) { acquire(p); }
Reference::Reference(const Reference &r): Reference(r.pointer) {}
Reference::~Reference() { release(pointer); }
Reference &Reference::operator=(const Reference &r) {
  acquire(r.pointer);
  release(pointer);
  pointer = r.pointer;
  return *this;
}
Object *Reference::operator->() const {
  return pointer;
}

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

}
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
