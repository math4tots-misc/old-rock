#ifndef rock_value_class_hh
#define rock_value_class_hh

#include <functional>
#include <map>
#include <vector>

#include <iostream>

#include "rock/value/value.hh"
#include "rock/base/result.hh"

namespace rock {

using ClassInitializer = std::function<std::map<std::string,Method>()>;

extern Reference<Class> classClass;


struct Class final: Value {
  const std::string name;
  const std::vector<Reference<Class>> bases;
  const std::map<std::string, Method> method_table;

  Class(const std::string& n): name(n) {}
  Class(const std::string& n, ClassInitializer i):
      name(n), method_table(i()) {}
  Class(const std::string& n, std::vector<Reference<Class>> bs):
      Class(n, bs, []() { return std::map<std::string, Method>(); }) {}
  Class(
      const std::string& n, std::vector<Reference<Class>> bs,
      ClassInitializer i):
          name(n), bases(bs), method_table(combine(bs, i())) {}
  Reference<Class> getClass() const override { return classClass; };
  std::string debugstr() const override {
    return "Class(" + name + ")";
  }

private:
  // Simple DFS multiple inheritance. Maybe switch to CRO if I
  // get really bored.
  std::map<std::string, Method>
  combine(std::vector<Reference<Class>> bs, std::map<std::string, Method> m) {
    std::map<std::string, Method> result;
    std::cout << "Hi" << std::endl;
    for (const Reference<Class>& b: bs) {
      for (const auto& pair: b->method_table) {
        if (result.find(pair.first) == result.end()) {
          result[pair.first] = pair.second;
        }
      }
    }
    for (const auto& pair: m) {
      result[pair.first] = pair.second;
    }
    return result;
  }
};

}

#endif//rock_value_class_hh
