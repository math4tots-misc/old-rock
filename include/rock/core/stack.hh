#ifndef rock_core_stack_hh
#define rock_core_stack_hh

#include <map>
#include <string>

namespace rock {

class Token;

extern std::vector<const Token*> *stack;

class StackFrame {
public:
  StackFrame(const Token *t);
  ~StackFrame();
};

}

#endif//rock_core_stack_hh
