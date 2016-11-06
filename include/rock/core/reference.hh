#ifndef rock_core_reference_hh
#define rock_core_reference_hh

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
  template <class T> T *as() const { return static_cast<T*>(pointer); }
};

}  // namespace rock

#endif//rock_core_reference_hh
