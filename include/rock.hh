#ifndef rock_hh
#define rock_hh

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace rock {

//** file

struct File {
  const std::string path, contents;
  File(const std::string& p, const std::string& c): path(p), contents(c) {}
};

struct Token {
  File *const file;
  const long position;
  Token(File *const f, long p): file(f), position(p) {}
};

//** stack

extern std::vector<Token*> stack;

struct StackFrame {
  StackFrame(Token *t) {
    stack.push_back(t);
  }
  ~StackFrame() {
    stack.pop_back();
  }
};

//** core

struct Reference;
struct Result;
struct Value;
struct Class;
struct Exception;
struct Nil;
struct String;

using Args = std::vector<Reference>;
using Method = std::function<Result(const Reference&, const Args&)>;

constexpr long NORMAL = 1;
constexpr long EXCEPTION = 2;
constexpr long RETURN = 3;
constexpr long BREAK = 4;
constexpr long CONTINUE = 5;

extern Class *classClass;
extern Class *classException;
extern Class *classNil;
extern Class *classString;

extern Nil *nil;

void acquire(Value *v);
void release(Value *v);

/** Reference is basically a pointer to a Value with reference counting.
 * The main way to interact with a reference is by calling methods on it
 * using the 'call' method.
 * The other main thing you can do is to directly access the Value pointer.
 */
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

/** The Result type is the result of any calculation performed in Rock.
 * We don't return just the Reference because there may be additional
 * context associated with it, e.g. whether we are returning an EXCEPTION.
 * If the result is just the value itself, type should be NORMAL.
 */
struct Result final {
  const long type;
  const Reference value;
  Result(long t, Reference r): type(t), value(r) {}
};

/** Value is the base class of all objects that are manipulated in a Rock
 * program.
 */
struct Value {
  long reference_count = 0;
  virtual ~Value() {}
  virtual Class *getClass()=0;
};

struct Class final: Value {
  std::map<std::string,Method*> method_table;
  Class *getClass() override { return classClass; }
};

struct Exception final: Value {
  const std::string message;
  Exception(const std::string& m): message(m) {}  // TODO: stacktrace
  Class *getClass() override { return classException; }
};

struct Nil final: Value {
  Class *getClass() override { return classNil; }
};

struct String final: Value {
  const std::string value;
  String(const std::string& s): value(s) {}
  Class *getClass() override { return classString; }
};

}

#endif//rock_hh
