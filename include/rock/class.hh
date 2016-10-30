#ifndef rock_class_hh
#define rock_class_hh

#include <map>
#include <string>
#include <vector>

#include "rock/object.hh"

namespace rock {

class Class;

extern Class *classClass;

class Class final: public Object {
public:
  const std::string name;
  const std::vector<Reference> bases;
private:
  std::map<std::string,Method> methods;
public:
  Class(const std::string&);
  Class(const std::string&, const std::vector<Reference>&);
  Class(const std::string&, const std::map<std::string,Method>&);
  Class(
      const std::string&,
      const std::vector<Reference>&,
      const std::map<std::string,Method>&);
  Reference getClass() const override;
  Method getMethod(const std::string&) const;
  std::string debug() const override;
};

}

#endif//rock_class_hh
