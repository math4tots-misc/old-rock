#ifndef rock_base_reference_hh
#define rock_base_reference_hh

#include "rock/base/referencecounted.hh"

namespace rock {

/**
 * The way to hold reference counted objects.
 */
template <class T>
struct Reference final {
  static_assert(
      std::is_base_of<ReferenceCounted, T>::value,
      "Reference objects must point to ReferenceCounted objects");
  T *pointer;
  Reference(T *p): pointer(p) { p->acquire(); }
  Reference(const Reference& r): Reference(r.pointer) {}
  Reference& operator=(const Reference& r) {
    r.pointer->acquire();
    pointer->release();
    pointer = r.pointer;
  }
  T *operator->() const { return pointer; }
  template <class K>
  operator Reference<K>() {
    return Reference<K>(pointer);
  }
};

}

#endif//rock_base_reference_hh
