#ifndef rock_core_reference_hh
#define rock_core_reference_hh

#include "rock/core/object.hh"

namespace rock {

class Object;

class Reference final {
  Object *pointer;
  static void acquire(Object*);
  static void release(Object*);
public:
  Reference();
  Reference(Object*);
  Reference(const Reference&);
  ~Reference();
  Reference &operator=(const Reference&);
  Object *operator->() const;
  bool is_not_null() const;
  bool is_null() const;
  bool operator==(const Reference&) const;
  template <class T> T *as() const { return static_cast<T*>(pointer); }
};

template <class T>
Method::Method(T functor):
    data(new T(functor)),
    impl([](void *data, Reference r, Class *c, const Args &args) -> Reference {
      return (*static_cast<T*>(data))(r, c, args);
    }) {
  std::cout << typeid(T).name() << std::endl;
}

template <>
Method::Method(std::nullptr_t):
  Method(static_cast<Reference(*)(Reference,Class*,const Args&)>(nullptr)) {}

// template <>
// Method::Method(Reference(*f)(Reference,Class*,const Args&)):
//     Method(f, [](void *d, Reference r, Class *c, const Args &args) {
//       return static_cast<Implementation>(d)(r, c, args);
//     }) {}

}  // namespace rock

#endif//rock_core_reference_hh
