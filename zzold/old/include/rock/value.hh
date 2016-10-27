#ifndef rock_value_hh
#define rock_value_hh

#include <string>
#include <vector>
#include <iostream>

namespace rock {

struct Class;
struct Reference;
using Args = std::vector<Reference>;

extern std::vector<Reference> vals;

struct Value {
  long reference_count = 0;
  Value();
  virtual ~Value() {}
  virtual Class *getClass()=0;

  // Just for debugging...
  virtual std::string debugstr() const {
    return std::string("<") + typeid(*this).name() + " instance>";
  }
};

}

#endif//rock_value_hh
