#ifndef rock_builtins_hh
#define rock_builtins_hh

#include <unordered_map>

#include "rock/scope.hh"

namespace rock {

struct Globals: Scope {
  Globals(): Scope() {
    set("String", P(&classString));
  }
};

}

#endif//rock_builtins_hh
