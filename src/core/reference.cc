#include "rock/core/all.hh"

#include <sstream>
#include <iostream>

#include <csignal>

namespace rock {

std::vector<const Token*> getStackTrace() {
  std::vector<const Token*> trace;
  for (const Token *t: *stack) {
    trace.push_back(t);
  }
  return trace;
}

void Reference::acquire(Object *pointer) {
  if (pointer) {
    pointer->refcnt++;
  }
}

void Reference::release(Object *pointer) {
  if (pointer) {
    pointer->refcnt--;
    if (pointer->refcnt <= 0) {
      delete pointer;  // Let everything leak for now
    }
  }
}

Reference::Reference(): pointer(nullptr) {}
Reference::Reference(Object *p): pointer(p) { acquire(p); }
Reference::Reference(const Reference &r): Reference(r.pointer) {}
Reference::~Reference() { release(pointer); }
Reference &Reference::operator=(const Reference &r) {
  acquire(r.pointer);
  release(pointer);
  pointer = r.pointer;
  return *this;
}
Object *Reference::operator->() const {
  return pointer;
}

}  // namespace rock
