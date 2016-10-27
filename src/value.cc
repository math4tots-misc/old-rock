#include "rock/all.hh"

namespace rock {

Result<Ref> Value::call(const std::string& name, const Args& args) {
  Reference<Class> cls = getClass();
  auto pair = cls->method_table.find(name);
  if (pair == cls->method_table.end()) {
    return Result<Ref>(EXCEPTION, new Exception("No such method: " + name));
  }
  Method method = pair->second;
  return method(this, args);
}

}
