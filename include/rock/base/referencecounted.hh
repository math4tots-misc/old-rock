#ifndef rock_base_referencecounted_hh
#define rock_base_referencecounted_hh

#include "rock/base/debuggable.hh"

namespace rock {

/**
 * Abstract base class for anything reference counted.
 */
struct ReferenceCounted: Debuggable {
  long refcnt = 0;
  virtual ~ReferenceCounted() {}
  void acquire() {
    refcnt++;
  }
  void release() {
    if (--refcnt <= 0) {
      delete this;
    }
  }
};

}

#endif//rock_base_referencecounted_hh
