#ifndef rock_scope_hh
#define rock_scope_hh

#include <unordered_map>

#include "rock/value/value.hh"

namespace rock {

struct Scope {
  Scope *parent;
  std::unordered_map<std::string, P> vars;

  Scope(): Scope(nullptr) {}
  Scope(Scope *p): parent(p) {}

  Scope& set(std::string name, P value) {
    vars[name] = value;
    return *this;
  }

  bool has(std::string name) const {
    return
      vars.find(name) != vars.end() ||
      (parent != nullptr && parent->has(name));
  }

  P get(std::string name) const {
    auto iterp = vars.find(name);
    if (iterp != vars.end()) {
      return iterp->second;
    }
    return parent->get(name);  // TODO: nullptr check
  }
};

}

#endif//rock_scope_hh
