#include "rock/all.hh"

namespace rock {

Class *classClass;

namespace {
Init init(110, __FILE__, []() {
  classClass = new Class("Class");
  builtins->declare("Class", classClass);
});
}

Class::Class(const std::string &n): Class(n, {}, {}) {}
Class::Class(const std::string &n, const std::vector<Reference> &bs):
    Class(n, bs, {}) {}
Class::Class(const std::string &n, const std::map<std::string,Method> &ms):
    Class(n, {}, ms) {}
Class::Class(
    const std::string &n,
    const std::vector<Reference> &bs,
    const std::map<std::string,Method> &ms):
        Class(n, bs, false, {}, ms) {}
Class::Class(
    const std::string &n,
    const std::vector<Reference> &bs,
    bool uc,
    const std::set<std::string> &fs,
    const std::map<std::string,Method> &ms):
        name(n), bases(bs), userConstructible(uc), fields(fs), methods(ms) {}

Reference Class::getClass() const {
  return classClass;
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

}
