#ifndef rock_stack_hh
#define rock_stack_hh

#include <map>
#include <string>

namespace rock {

class Token;

extern std::vector<Token*> *stack;

class StackFrame {
public:
  StackFrame(Token *t);
  ~StackFrame();
};

}

#endif//rock_stack_hh
