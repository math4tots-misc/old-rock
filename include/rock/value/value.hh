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

  virtual std::unordered_map<std::string,Method>* getMeta()=0;

  Result call(const std::string& name, const std::vector<Value*>& args) {
    incref();
    for (Value *arg: args) {
      arg->incref();
    }
    auto meta = getMeta();
    auto pair = meta->find(name);
    if (pair == meta->end()) {
      decref();  // TODO: Factor
      for (Value *arg: args) {
        arg->decref();
      }
      return Result(EXCEPTION, "No such method: " + name);
    }
    Result result = pair->second->call(args);
    decref();  // TODO: Factor
    for (Value *arg: args) {
      arg->decref();
    }
    return result;
  }
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

#endif//rock_value_value_hh
