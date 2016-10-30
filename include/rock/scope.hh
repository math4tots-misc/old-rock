#ifndef rock_scope_hh
#define rock_scope_hh

#include <map>
#include <string>

namespace rock {

class Reference;

class Scope final {
  Scope *const parent;
  std::map<std::string, Reference> bindings;
public:
  Scope();
  Scope(Scope*);
  Result get(const std::string&);
  Result set(const std::string&, Reference);
  Result declare(const std::string&);
  Result declare(const std::string&, Reference);
};

}

#endif//rock_scope_hh
