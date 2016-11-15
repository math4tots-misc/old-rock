#include "rock/core/all.hh"

#include <sstream>

#include <iostream>

namespace rock {

namespace {
Init init(100, __FILE__, []() {
  classObject = new Class("Object", {}, {
    {"__init", [](Reference, Class*, const Args &args) {
      checkargs(0, args);
      return nil;
    }},
    {"__ne", [](Reference owner, Class*, const Args &args) {
      return owner->call("__eq", args)->truthy() ? xfalse : xtrue;
    }},
    {"__is", [](Reference owner, Class*, const Args &args) {
      checkargs(1, args);
      return Bool::from(owner.as<Object>() == args[0].as<Object>());
    }},
    {"__is_not", [](Reference owner, Class*, const Args &args) {
      checkargs(1, args);
      return Bool::from(owner.as<Object>() != args[0].as<Object>());
    }},
    {"to", [](Reference owner, Class*, const Args &args) {
      checkargs(1, args);
      checkinstance(classClass, args[0]);
      Class *cls = owner->getClass().as<Class>();
      Class *c = args[0].as<Class>();
      if (c == classString) {
        return owner->call("__str", {});
      } else if (c == builtins->get("Iterator").as<Object>()) {
        return owner->call("__iter", {});
      }
      throw exception(
          "No known conversion from " + cls->name + " to " + c->name);
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
  Class *c = cls->findClassWithMethod(name);
  if (!c) {
    const std::string message =
        "No such method '" + name + "' for class '" + cls->name + "'";
    throw exception(message);
  }
  Method method = c->getDirectMethod(name);
  return method(this, c, args);
}

Reference
Object::callSuper(Class *cp, const std::string &name, const Args &args) {
  Reference clsref = getClass();
  Class *cls = clsref.as<Class>();
  Class *c = cls->findClassWithMethod(cp, name);
  if (!c) {
    const std::string message =
        "No such super method '" + name +
        "' for class '" + cls->name + "' (beyond " + cp->name + ")";
    throw exception(message);
  }
  Method method = c->getDirectMethod(name);
  return method(this, c, args);
}

bool Object::hasMethod(const std::string &name) const {
  Reference clsref = getClass();
  Class *cls = clsref.as<Class>();
  return (bool) cls->findClassWithMethod(name);
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
