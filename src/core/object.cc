#include "rock/core/all.hh"

#include <sstream>

namespace rock {

namespace {
Init init(100, __FILE__, []() {
  classObject = new Class("Object", {}, {
    {"__init", [](Reference, const Args &args) {
      checkargs(0, args);
      return nil;
    }},
    {"__ne", [](Reference owner, const Args &args) {
      return owner->call("__eq", args)->truthy() ? xfalse : xtrue;
    }},
    {"__is", [](Reference owner, const Args &args) {
      checkargs(1, args);
      return Bool::from(owner.as<Object>() == args[0].as<Object>());
    }},
    {"__is_not", [](Reference owner, const Args &args) {
      checkargs(1, args);
      return Bool::from(owner.as<Object>() != args[0].as<Object>());
    }},
  });
  builtins->declare("Object", classObject);
});
}

Class *classObject;

Object::Object() {}

Object::~Object() {}

Reference Object::call(const std::string &name, const Args &args) {
  Reference clsref = getClass();
  Class *cls = clsref.as<Class>();
  Method method = cls->getMethod(name);
  if (!method) {
    const std::string message =
        "No such method '" + name + "' for class '" + cls->name + "'";
    throw exception(message);
  }
  return method(this, args);
}

bool Object::hasMethod(const std::string &name) const {
  Reference clsref = getClass();
  Class *cls = clsref.as<Class>();
  return (bool) cls->getMethod(name);
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
