#ifndef rock_value_value_hh
#define rock_value_value_hh

#include <vector>
#include <string>

#include "rock/base/reference.hh"
#include "rock/base/referencecounted.hh"

namespace rock {

struct Class;
struct Value;
template <class T> struct Result;

using Ref = Reference<Value>;
using Args = std::vector<Ref>;
using Method = std::function<Result<Ref>(Ref, Args)>;

/**
 * Base class for rock objects.
 */
struct Value: ReferenceCounted {
  virtual ~Value() {}
  virtual Reference<Class> getClass() const=0;
  Result<Ref> call(const std::string& name, const Args& args);
};

}

#endif//rock_value_value_hh
