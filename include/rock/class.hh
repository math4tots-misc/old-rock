#ifndef rock_class_hh
#define rock_class_hh

#include <map>
#include <string>
#include <functional>

#include "rock/value.hh"
#include "rock/reference.hh"

namespace rock {

struct Class;

using Args = std::vector<Reference>;
using Method = std::function<Result(Reference, const Args&)>;

extern Class *classClass;

struct Class final: Value {
  const std::string name;
  std::map<std::string,Method> method_table;
  Class(const std::string& n): name(n) {}
  Class *getClass() override { return classClass; }
  std::string debugstr() const override { return "<Class " + name + ">"; }
};

}

#endif//rock_class_hh
