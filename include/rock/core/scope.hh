#ifndef rock_core_scope_hh
#define rock_core_scope_hh

#include <map>
#include <string>

#include "rock/core/object.hh"

namespace rock {

extern Class *classScope;

class Scope final: public Object {
  Reference parent;
  std::map<std::string, Reference> bindings;
public:
  Scope();
  Scope(Scope*);
  Reference get(const std::string&);
  Reference set(const std::string&, Reference);
  Reference declare(const std::string&);
  Reference declare(const std::string&, Reference);
  void erase(const std::string&);
  Reference getClass() const override;
  std::map<std::string, Reference> getBindings() const;
};

}

#endif//rock_core_scope_hh
