#ifndef rock_osx_platform_hh
#define rock_osx_platform_hh

#include "rock/core/platform.hh"

namespace rock {

// Contains information that the process can use to ask questions about
// the process.
class OsxPlatform final: public Platform {
public:
  virtual std::string getName() const;
};

}

#endif//rock_osx_platform_hh
