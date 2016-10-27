#include "rock/reference.hh"

#include "rock/value.hh"
#include "rock/nil.hh"
#include "rock/result.hh"
#include "rock/class.hh"
#include "rock/exception.hh"

namespace rock {

Reference::Reference(): Reference(nil) {}

Result Reference::call(const std::string& name, const Args& args) const {
  Class *cls = pointer->getClass();
  auto pair = cls->method_table.find(name);
  if (pair == cls->method_table.end()) {
    return Result(
        EXCEPTION,
        Reference(new Exception("No such method: " + name)));
  }
  Method method = pair->second;
  return method(*this, args);
}

std::string Reference::debugstr() const {
  return pointer->debugstr();
}

void Reference::acquire(Value *v) {
  v->reference_count++;
}

void Reference::release(Value *v) {
  v->reference_count--;
  if (v->reference_count <= 0) {
    delete v;
  }
}

}
