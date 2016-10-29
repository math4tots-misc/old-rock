#ifndef rock_hh
#define rock_hh

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <initializer_list>

namespace rock {

struct Object;
struct Reference;
struct Result;

using Args = std::vector<Reference>;
using Method = std::function<Result(Reference, Args)>;

struct Object final {

  enum class Type {
    nil, boolean, number, string, list, map, function, cls
  };

  struct Class final {
    const std::string name;
    const std::vector<Class*> bases;
    const std::map<std::string, Method> methods;
    Class(
        const std::string& n,
        const std::vector<Class*> bs,
        const std::map<std::string, Method>& ms):
            name(n), bases(bs), methods(ms) {}

    Method getMethod(const std::string& name) const {
      auto pair = methods.find(name);
      if (pair != methods.end()) {
        return pair->second;
      }
      for (const auto& base: bases) {
        auto pair = base->methods.find(name);
        if (pair != base->methods.end()) {
          return pair->second;
        }
      }
      return nullptr;
    }
  };

  struct Function final {
    const std::string name;
    const std::function<Result(const Args&)> call;
    Function(const std::string& n, std::function<Result(const Args&)> f):
        name(n), call(f) {}
  };

  long refcnt = 0;
  const Type type;
  union {
    bool boolean;
    double number;
    std::string string;
    std::vector<Reference> list;
    std::map<std::string, Reference> map;
    Function function;
    Class cls;
  };

  Object(): type(Type::nil) {}
  Object(bool b): type(Type::boolean), boolean(b) {}
  Object(long n): type(Type::number), number(n) {}
  Object(double n): type(Type::number), number(n) {}
  Object(const std::string& s): type(Type::string), string(s) {}
  Object(std::initializer_list<Reference> r): type(Type::list), list(r) {}
  Object(const std::string& name, std::function<Result(const Args&)> f):
      type(Type::function), function(name, f) {}

  ~Object() {
    switch (type) {
    case Type::nil: break;
    case Type::boolean: break;
    case Type::number: break;
    case Type::string: string.~basic_string(); break;
    case Type::list: list.~vector(); break;
    case Type::map: map.~map(); break;
    case Type::function: function.~Function(); break;
    case Type::cls: cls.~Class(); break;
    }
  }

  void acquire() {
    refcnt++;
  }

  void release() {
    if (--refcnt <= 0) {
      delete this;
    }
  }
};

struct Reference final {
  Object *pointer;
  Reference();
  Reference(Object *p): pointer(p) { p->acquire(); }
  Reference(const Reference& r): Reference(r.pointer) {}
  Reference& operator=(const Reference& r) {
    r.pointer->acquire();
    pointer->release();
    pointer = r.pointer;
    return *this;
  }
  Object *operator->() const { return pointer; }
};

struct Result final {

  enum class Type {
    normal, exception
  };

  const Type type;
  Reference value;

  Result(): type(Type::exception) {}
  Result(const Reference& r): type(Type::normal), value(r) {}
};

}

#endif//rock_hh
