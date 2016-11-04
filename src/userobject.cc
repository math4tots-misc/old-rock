#include "rock/all.hh"

namespace rock {

UserObject::UserObject(Class *c): cls(c) {
  if (!c->userConstructible) {
    throw Reference(new Exception(
        "Class " + c->name + " is not constructible"));
  }
  for (const std::string &fieldName: c->fields) {
    fields[fieldName] = nil;
  }
}

Reference UserObject::getClass() const {
  return cls;
}

}
