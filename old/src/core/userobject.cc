#include "rock/core/all.hh"

#include <sstream>

namespace rock {

UserObject::UserObject(Class *c): cls(c) {
  if (!c->userConstructible) {
    throw Reference(new Exception(
        "Class " + c->name + " is not constructible"));
  }
  for (const std::string &fieldName: c->getFields()) {
    fields[fieldName] = nil;
  }
}

Reference UserObject::getClass() const {
  return cls;
}

std::string UserObject::str() const {
  if (hasMethod("__str")) {
    // NOTE: BAD!!! But no way around it really AFAICT...
    return const_cast<UserObject*>(this)->call("__str", {})->str();
  }
  std::stringstream ss;
  ss << getClass().as<Class>()->name << "@" << this;
  return ss.str();
}

Reference UserObject::setField(const std::string &name, Reference value) {
  if (fields.find(name) == fields.end()) {
    throw exception("No such field (set): " + name);
  }
  fields[name] = value;
  return value;
}

Reference UserObject::getField(const std::string &name) const {
  auto pair = fields.find(name);
  if (pair == fields.end()) {
    throw exception("No such field (set): " + name);
  }
  return pair->second;
}

}
