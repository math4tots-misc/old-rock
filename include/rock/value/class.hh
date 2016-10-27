#ifndef rock_value_class_hh
#define rock_value_class_hh

#include <functional>
#include <vector>

#include "rock/value/value.hh"
#include "rock/base/result.hh"

namespace rock {

using Args = std::vector<Ref>;
using Method = std::function<Ref(Ref, Args)>;

extern Reference<Class> classClass;


struct Class final: Value {
  const std::string name;
  Class(const std::string& n): name(n) {}
  Reference<Class> getClass() const override { return classClass; };
  std::string debugstr() const override {
    return "Class(" + name + ")";
  }
};

}

#endif//rock_value_class_hh
