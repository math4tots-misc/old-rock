#ifndef rock_value_function_hh
#define rock_value_function_hh

#include <vector>

#include "rock/value/class.hh"

namespace rock {

struct Function: Value {
  size_t hash() override { return reinterpret_cast<size_t>(this); }
  virtual P call(const std::vector<P>& args)=0;
};

}

#endif//rock_value_function_hh
