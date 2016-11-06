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
namespace {

// NOTE: DEBUG
// std::map<Object*,std::string> id;
// std::map<Object*,long> refcnts;
// std::map<Object*,std::vector<const Token*>> deleted;

}

void Reference::acquire(Object *pointer) {
  if (pointer) {
    // NOTE: DEBUG
    // if (refcnts.find(pointer) == refcnts.end()) {
    //   id[pointer] = pointer->debug();
    //   refcnts[pointer] = 0;
    // }
    // if (allObjects.find(pointer) == allObjects.end()) {
    //   std::cerr << "Object does not exist! " << id[pointer] << std::endl;
    //   exit(1);
    // }
    // refcnts[pointer]++;
    // std::cerr << id[pointer] << " (++) -> " << refcnts[pointer] << std::endl;
    pointer->refcnt++;
  }
}

void Reference::release(Object *pointer) {
  if (pointer) {
    pointer->refcnt--;
    // NOTE: DEBUG
    // refcnts[pointer]--;
    // std::cerr << id[pointer] << " (--) -> " << refcnts[pointer] << std::endl;
    if (pointer->refcnt <= 0) {
      // NOTE: DEBUG
      // if (id[pointer] == "<UserFunction @core/__prelude.rock:25>") {
      //   std::cerr << (new Exception("XX"))->str() << std::endl;
      //   // std::raise(SIGINT);  // Force a breakpoint
      // }
      // // TODO: There's a problem with memory management. In particular,
      // // if a reference is hidden away inside an std::function object,
      // // there's no real way for me to get at it at the moment.
      // std::cerr << "Deleting: " << id[pointer] << std::endl;
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
