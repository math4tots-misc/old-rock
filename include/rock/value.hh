#ifndef rock_value_hh
#define rock_value_hh

#include <unordered_map>
#include <string>

namespace rock {

struct Class;

struct Value {
  long refcnt = 0;

  virtual Class& getClass() const=0;
  virtual size_t hash()=0;
  virtual ~Value() {}
};

inline void incref(Value *v) {
  if (v) {
    v->refcnt++;
  }
}

inline void decref(Value *v) {
  if (v) {
    v->refcnt--;
    if (v->refcnt <= 0) {
      delete v;
    }
  }
}

struct P final {
  Value *ptr;

  P();  // NOTE: defined in class.hh (set to nil)
  P(Value *p) : ptr(p) { incref(ptr); }
  ~P() { decref(ptr); }
  P(const P& p): P(p.ptr) {}
  P& operator=(const P& p) {
    incref(p.ptr);
    decref(ptr);
    ptr = p.ptr;
    return *this;
  }
  Value *operator->() const { return ptr; }
};

}

namespace std {
template <>
struct hash<rock::P> {
  size_t operator()(const rock::P& p) const {
    return p->hash();
  }
};
}

#endif//rock_value_hh
