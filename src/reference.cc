#include "rock/all.hh"

#include <sstream>

namespace rock {

void Reference::acquire(Object *pointer) {
  if (pointer) {
    pointer->refcnt++;
  }
}

void Reference::release(Object *pointer) {
  if (pointer) {
    pointer->refcnt--;
    if (pointer->refcnt <= 0) {
      delete pointer;
      pointer = nullptr;
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
