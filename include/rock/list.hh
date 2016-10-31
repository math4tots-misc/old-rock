#ifndef rock_list_hh
#define rock_list_hh

#include <string>

#include "rock/object.hh"

namespace rock {

class Class;

extern Class *classList;

class List final: public Object {
public:
  std::vector<Reference> value;
  List(const std::vector<Reference>&);
  Reference getClass() const override;
  std::string debug() const override;
  std::string str() const override;
};

}

#endif//rock_list_hh
