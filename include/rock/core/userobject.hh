#ifndef rock_core_userobject_hh
#define rock_core_userobject_hh

#include <string>

#include "rock/core/object.hh"

namespace rock {

class UserObject final: public Object {
  std::map<std::string,Reference> fields;
public:
  const Reference cls;
  UserObject(Class*);
  Reference getClass() const override;
  std::string str() const override;
  Result setField(const std::string&, Reference);
  Result getField(const std::string&) const;
};

}

#endif//rock_core_userobject_hh
