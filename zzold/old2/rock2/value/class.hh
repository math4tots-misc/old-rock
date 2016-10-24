#ifndef rock_value_class_hh
#define rock_value_class_hh

#include "rock/value/value.hh"

namespace rock {

extern Class classClass;

struct Class final: Value {
  std::string name;
  std::unordered_map<std::string,P> method_table;

  Class(const std::string& n): name(n) {
    incref(this);  // live forever
  }
  Class& getClass() const override { return classClass; }
  size_t hash() override { return reinterpret_cast<size_t>(this); }
};

Class classClass("Class");  // TODO: cc

Class classNil("Nil");  // TODO: cc

struct Nil final: Value {
  Nil() { incref(this); }  // Live forever
  Class& getClass() const override { return classNil; }
  size_t hash() override { return 77; }
};

Nil nil;  // TODO: cc
P nilptr(&nil);  // TODO: cc

inline P::P(): P(&nil) {}

}

#endif//rock_value_class_hh
