#include "rock/core/all.hh"

namespace rock {

// Method::Method(void *d, Reference(*f)(Reference,Class*,const Args&)):
//     data(d), impl(f) {}

Reference Method::operator()(Reference r, Class *c, const Args &args) {
  return impl(r, c, args);
}

Method::operator bool() const {
  return (bool) impl;
}

}  // namespace rock
