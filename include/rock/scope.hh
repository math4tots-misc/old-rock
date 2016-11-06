#ifndef rock_scope_hh
#define rock_scope_hh

#include <map>
#include <string>

#include "rock/object.hh"

namespace rock {

extern Class *classScope;

class Scope final: public Object {
  Scope *const parent;
  std::map<std::string, Reference> bindings;
public:
  Scope();
  Scope(Scope*);
  Result get(const std::string&);
  Result set(const std::string&, Reference);
  Result declare(const std::string&);
  Result declare(const std::string&, Reference);
  Reference getClass() const override;
  std::map<std::string, Reference> getBindings() const;
};

}

#endif//rock_scope_hh
