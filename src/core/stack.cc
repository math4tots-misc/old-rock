#include "rock/core/all.hh"

namespace rock {


std::vector<const Token*> *stack;

namespace {
Init init(100, __FILE__, []() {
  stack = new std::vector<const Token*>();
}, []() {
  delete stack;
});
}

StackFrame::StackFrame(const Token *t) {
  stack->push_back(t);
}

StackFrame::~StackFrame() {
  stack->pop_back();
}

}
