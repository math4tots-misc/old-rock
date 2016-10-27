#ifndef rock_stack_hh
#define rock_stack_hh

namespace rock {

struct Token;

extern std::vector<Token*> stack;

struct StackFrame final {
  StackFrame(Token *t) {
    stack.push_back(t);
  }
  ~StackFrame() {
    stack.pop_back();
  }
};

}

#endif//rock_stack_hh
