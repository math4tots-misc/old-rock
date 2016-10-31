#include "rock/all.hh"

namespace rock {


std::vector<Token*> *stack;

namespace {
Init init(100, __FILE__, []() {
  stack = new std::vector<Token*>();
}, []() {
  delete stack;
});
}

StackFrame::StackFrame(Token *t) {
  stack->push_back(t);
}

StackFrame::~StackFrame() {
  stack->pop_back();
}

}
