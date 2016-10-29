#ifndef rock_base_debuggable_hh
#define rock_base_debuggable_hh

#include <string>

namespace rock {

/**
 * Abstract base class for a lot of other classes to make debugging easier.
 */
struct Debuggable {
  virtual std::string debugstr() const=0;
};

}

#endif//rock_base_debuggable_hh
