#ifndef rock_object_hh
#define rock_object_hh

#include <functional>
#include <vector>

namespace rock {

class Object;
class Reference;
class Result;

using Args = std::vector<Reference>;
using Method = std::function<Result(Reference, const Args&)>;

void acquire(Object*);
void release(Object*);

class Object {
  friend void acquire(Object*);
  friend void release(Object*);
  int refcnt = 0;
public:
  virtual ~Object() {}
  Result call(const std::string&, const Args&);
  virtual Reference getClass() const=0;
  virtual std::string debug() const;
};

class Reference final {
  Object *pointer;
public:
  Reference();
  Reference(Object*);
  Reference(const Reference&);
  ~Reference();
  Reference &operator=(const Reference&);
  Object *operator->() const;
  template <class T> T *as() const { return static_cast<T*>(pointer); }
};

class Result {
public:
  enum class Type {
    OK, EXCEPTION, RETURN, BREAK, CONTINUE,
  };
  const Type type;
  const Reference value;
  Result(Reference);
  Result(Type, Reference);
  Reference get() const;
};

}

#endif//rock_object_hh
