#ifndef rock_value_value_hh
#define rock_value_value_hh

#include <map>
#include <string>

#include "rock/exec/result.hh"

namespace rock {

struct Value;

struct Method {
  virtual Result call(Value* owner, const std::vector<Value*>& args)=0;
};

struct Class;

struct Value {
  long refcnt = 0;

  virtual ~Value() {}
  void decref() {
    refcnt--;
    if (refcnt <= 0) {
      delete this;
    }
  }
  void incref() {
    refcnt++;
  }
  virtual Class *getClass()=0;
  virtual std::unordered_map<std::string,Method>* getMeta()=0;
  /** NOTE: Whoever calls 'call' should guarantee that 'this' and
   * every element of 'args' is owned by the caller. Otherwise objects
   * may be freed early and lead to weird behavior.
   */
  Result call(const std::string& name, const std::vector<Value*>& args) {
    auto meta = getMeta();
    auto pair = meta->find(name);
    if (pair == meta->end()) {
      return Result(EXCEPTION, new Exception("No such method: " + name));
    }
    return pair->second->call(args);
  }
};
}

namespace std {
template <>
struct hash<rock::Value*> {
  size_t operator()(rock::Value* p) const {
    return p->hash();
  }
};
}

#endif//rock_value_value_hh
