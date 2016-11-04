#include "rock/all.hh"

#include <sstream>

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

std::string UserObject::str() const {
  if (hasMethod("__str")) {
    // NOTE: BAD!!! But no way around it really AFAICT...
    Result result = const_cast<UserObject*>(this)->call("__str", {});
    if (result.type != Result::Type::OK) { throw result; }
    return result.value->str();
  }
  std::stringstream ss;
  ss << getClass().as<Class>()->name << "@" << this;
  return ss.str();
}

Result UserObject::setField(const std::string &name, Reference value) {
  if (fields.find(name) == fields.end()) {
    return Result(
        Result::Type::EXCEPTION,
        new Exception("No such field (set): " + name));
  }
  fields[name] = value;
  return Result(Result::Type::OK, value);
}

Result UserObject::getField(const std::string &name) const {
  auto pair = fields.find(name);
  if (pair == fields.end()) {
    return Result(
        Result::Type::EXCEPTION,
        new Exception("No such field (set): " + name));
  }
  return Result(Result::Type::OK, pair->second);
}

}
