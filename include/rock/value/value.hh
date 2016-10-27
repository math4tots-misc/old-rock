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

extern Reference<Class> classValue;

/**
 * Base class for rock objects.
 */
struct Value: ReferenceCounted {
  Value() {
    std::cout << "A value is getting created!" << std::endl;
  }
  virtual ~Value() {}
  virtual Reference<Class> getClass() const=0;

  // Call a rock method.
  Result<Ref> call(const std::string& name, const Args& args);
};

}

#endif//rock_value_value_hh
