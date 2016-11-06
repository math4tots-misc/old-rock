#ifndef rock_core_class_hh
#define rock_core_class_hh

#include <map>
#include <set>
#include <string>
#include <vector>

#include "rock/core/object.hh"

namespace rock {

class Class;

extern Class *classClass;

bool instanceof(Reference, const Class*);

class Class final: public Object {
public:
  const std::string name;
  const std::vector<Reference> bases;

  /** User constructible means that you can create an instance of this
   * type by calling a constructor in Rock.
   * Most builtin types are *not* user constructible. To create them,
   * you must either use special functions or use special syntaxes.
   *
   * If a class is user constructible, you can create one by calling the
   * '__call', method on the class object.
   */
  const bool userConstructible;
  const std::set<std::string> fields;
  const std::map<std::string,Method> methods;
public:
  Class(
      const std::string&,
      const std::vector<Reference>&,
      const std::map<std::string,Method>&);
  Class(
      const std::string&,
      const std::vector<Reference>&,
      bool,
      const std::set<std::string>&,
      const std::map<std::string,Method>&);
  Reference getClass() const override;
  Method getMethod(const std::string&) const;
  std::string debug() const override;
};

}

#endif//rock_core_class_hh
