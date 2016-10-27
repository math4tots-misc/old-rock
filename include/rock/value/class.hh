#ifndef rock_value_class_hh
#define rock_value_class_hh

#include <functional>
#include <map>
#include <vector>

#include "rock/value/value.hh"
#include "rock/base/result.hh"

namespace rock {

extern Reference<Class> classClass;


struct Class final: Value {
  const std::string name;
  const std::map<std::string, Method> method_table;
  Class(const std::string& n): name(n) {}
  Reference<Class> getClass() const override { return classClass; };
  std::string debugstr() const override {
    return "Class(" + name + ")";
  }
};

}

#endif//rock_value_class_hh
