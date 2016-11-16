#ifndef rock_value_hh
#define rock_value_hh

#include <vector>
#include <string>
#include <map>
#include <type_traits>

namespace rock {

struct Value;
struct Object;
struct Class;

using Args = std::vector<Value>;

struct Value final {

  enum class Type {
    BOOL,
    NUMBER,
    OBJECT,
  };

  Type type;
  union {
    bool bewl;
    double num;
    Object *ptr;
  };

  Value();
  Value(bool);
  Value(int);
  Value(long);
  Value(double);
  Value(Object*);
  Value call(Class*, const std::string&, const Args&);
  bool truthy();
  bool operator==(Value) const;
};

static_assert(
    std::is_standard_layout<Value>::value,
    "Value class should be standard layout");

}  // namespace rock

#endif//rock_value_hh
