#ifndef rock_value_value_hh
#define rock_value_value_hh

#include "rock/base/reference.hh"
#include "rock/base/referencecounted.hh"

namespace rock {

struct Class;
struct Value;

using Ref = Reference<Value>;

/**
 * Base class for rock objects.
 */
struct Value: ReferenceCounted {
  virtual ~Value() {}
  virtual Reference<Class> getClass() const=0;
};

}

#endif//rock_value_value_hh
