#ifndef rock_function_hh
#define rock_function_hh

#include <vector>

#include "rock/class.hh"

namespace rock {

struct Function: Value {
  size_t hash() override { return reinterpret_cast<size_t>(this); }
  virtual P call(const std::vector<P>& args)=0;
};

}

#endif//rock_function_hh
