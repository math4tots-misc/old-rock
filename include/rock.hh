#ifndef rock_hh
#define rock_hh

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace rock {

struct Reference;
struct Result;
struct Value;
struct Class;
struct Exception;

using Args = std::vector<Reference>;
using Method = std::function<Result(const Reference&, const Args&)>;

constexpr long NORMAL = 1;
constexpr long EXCEPTION = 2;
constexpr long RETURN = 3;
constexpr long BREAK = 4;
constexpr long CONTINUE = 5;

void acquire(Value *v);
void release(Value *v);

struct Reference final {
  Value *pointer;
  Reference(Value *p): pointer(p) { acquire(pointer); }
  Reference(const Reference& r): Reference(r.pointer) {}
  ~Reference() { release(pointer); }
  Reference& operator=(const Reference& r) {
    acquire(r.pointer);
    release(pointer);
    pointer = r.pointer;
    return *this;
  }
  Result call(const std::string& name, const Args& args) const;
};

struct Result final {
  const long type;
  const Reference value;
  Result(long t, Reference r): type(t), value(r) {}
};

struct Value {
  long reference_count = 0;
  virtual ~Value() {}
  virtual Class *getClass()=0;
};

extern Class *classClass;

struct Class final: Value {
  std::map<std::string,Method*> method_table;
  Class *getClass() override { return classClass; }
};

extern Class *classException;

struct Exception final: Value {
  const std::string message;
  Exception(const std::string& m): message(m) {}  // TODO: stacktrace
  Class *getClass() override { return classException; }
};

// --- cc ---

Class *classClass = new Class();
Reference classClassReference(classClass);

Class *classException = new Class();
Reference classExceptionReference(classException);

void acquire(Value *v) {
  v->reference_count++;
}

void release(Value *v) {
  v->reference_count--;
  if (v->reference_count <= 0) {
    delete v;
  }
}

Result Reference::call(const std::string& name, const Args& args) const {
  Class *cls = pointer->getClass();
  auto pair = cls->method_table.find(name);
  if (pair == cls->method_table.end()) {
    return Result(
        EXCEPTION,
        Reference(new Exception("No such method: " + name)));
  }
  Method *method = pair->second;
  return (*method)(*this, args);
}

}

#endif//rock_hh