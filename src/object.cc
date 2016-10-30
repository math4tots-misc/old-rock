#include "rock/all.hh"

#include <sstream>

namespace rock {

void acquire(Object *pointer) {
  if (pointer) {
    pointer->refcnt++;
  }
}

void release(Object *pointer) {
  if (pointer) {
    pointer->refcnt--;
    if (pointer->refcnt <= 0) {
      delete pointer;
      pointer = nullptr;
    }
  }
}

Result Object::call(const std::string &name, const Args &args) {
  Reference clsref = getClass();
  Class *cls = clsref.as<Class>();
  Method method = cls->getMethod(name);
  if (!method) {
    const std::string message =
        "No such method: " + name + " for class " + cls->name;
    // TODO: Return Result(EXCEPTION...), instead of throw
    // return Result(EXCEPTION, new Exception(message));
    throw message;
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
    throw "Result is not OK";
  }
  return value;
}

}
