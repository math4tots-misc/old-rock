#ifndef rock_class_hh
#define rock_class_hh

#include "rock/object.hh"

#include <vector>
#include <string>
#include <map>
#include <type_traits>

namespace rock {

extern Class *xClass;

struct Class final: public Object {
  std::string name;
  std::vector<Class*> bases;
  std::vector<Class*> mro;
  std::map<std::tuple<Class*, std::string>, Value> methods;

  Class *getClass() const override;
  void traverse(void*, void(*)(void*,Object*)) const override;
};

}  // namespace rock

#endif//rock_class_hh
