#ifndef rock_exec_stack_hh
#define rock_exec_stack_hh

#include <vector>

#include "rock/file.hh"

namespace rock {

std::vector<const Token*> stack;  // TODO: cc

struct StackFrame {
  StackFrame(const Token& t) {
    stack.push_back(&t);
  }
  ~StackFrame() {
    stack.pop_back();
  }
};

}

#endif//rock_exec_stack_hh
