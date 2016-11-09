#include "rock/core/all.hh"

namespace rock {

Class *classClass;

namespace {
Init init(110, __FILE__, []() {
  classClass = new Class("Class", {classObject}, {
    {"__call", [](Reference owner, const Args &args) {
      Reference instance(new UserObject(owner.as<Class>()));
      instance->call("__init", args);
      return instance;
    }},
    {"extends", [](Reference owner, const Args &args) {
      checkargs(1, args);
      checktype(classClass, args[0]);
      Class *cls = owner.as<Class>();
      Class *base = args[0].as<Class>();
      return cls->extends(base) ? xtrue : xfalse;
    }},
    {"of", [](Reference owner, const Args &args) {
      checkargs(1, args);
      return args[0]->getClass();
    }},
  });
  builtins->declare("Class", classClass);
});
}

bool instanceof(Reference r, const Class *c) {
  return r->getClass().as<Class>() == c;
}

Class::Class(
    const std::string &n,
    const std::vector<Reference> &bs,
    const std::map<std::string,Method> &ms):
        Class(n, bs, false, {}, ms) {}
Class::Class(
    Reference c,
    const std::string &n,
    const std::vector<Reference> &bs,
    const std::map<std::string,Method> &ms):
        Class(c, n, bs, false, {}, ms) {}
Class::Class(
    const std::string &n,
    const std::vector<Reference> &bs,
    bool uc,
    const std::set<std::string> &fs,
    const std::map<std::string,Method> &ms):
        Class(nullptr, n, bs, uc, fs, ms) {}
Class::Class(
    Reference c,
    const std::string &n,
    const std::vector<Reference> &bs,
    bool uc,
    const std::set<std::string> &fs,
    const std::map<std::string,Method> &ms):
        cls(c), name(n), bases(bs), userConstructible(uc),
        fields(fs), methods(ms) {}

Reference Class::getClass() const {
  return cls.is_null() ? classClass : cls;
}

// TODO: Right now, getMethod does a simple DFS. Do MRO ordering.
Method Class::getMethod(const std::string &name) const {
  auto pair = methods.find(name);
  if (pair != methods.end()) {
    return pair->second;
  }
  for (const auto &base: bases) {
    Method method = base.as<Class>()->getMethod(name);
    if (method) {
      return method;
    }
  }
  return nullptr;
}

std::string Class::debug() const {
  return "<Class " + name + ">";
}

bool Class::extends(Class *base) const {
  if (this == base) {
    return true;
  }
  for (const Reference &b: bases) {
    if (b.as<Object>() == base) {
      return true;
    }
  }
  return false;
}

}
