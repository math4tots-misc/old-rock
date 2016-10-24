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
  Reference();  // NOTE: sets pointer to 'nil', NOT 'nullptr'
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
  virtual Class *getClass() noexcept=0;
};

struct Class final: Value {
  std::map<std::string,Method*> method_table;
  Class *getClass() noexcept override { return classClass; }
};

struct Exception final: Value {
  const std::string message;
  Exception(const std::string& m): message(m) {}  // TODO: stacktrace
  Class *getClass() noexcept override { return classException; }
};

struct Nil final: Value {
  Class *getClass() noexcept override { return classNil; }
};

struct String final: Value {
  const std::string value;
  String(const std::string& s): value(s) {}
  Class *getClass() noexcept override { return classString; }
};

//** scope

struct Scope final {
  Scope *const parent;
  std::map<std::string, Reference> bindings;
  Scope(Scope *p): parent(p) {}
  Result get(const std::string& name) const noexcept {
    auto pair = bindings.find(name);
    if (pair != bindings.end()) {
      return Result(NORMAL, pair->second);
    }
    if (!parent) {
      return Result(EXCEPTION, new Exception("No such variable: " + name));
    }
    return parent->get(name);
  }
  Result set(const std::string& name, Reference value) noexcept {
    auto pair = bindings.find(name);
    if (pair != bindings.end()) {
      bindings[name] = value;
      return Result(NORMAL, value);
    }
    if (!parent) {
      return Result(EXCEPTION, new Exception("No scuh variable: " + name));
    }
    return parent->set(name, value);
  }
  Result declare(const std::string& name, Reference value) noexcept {
    auto pair = bindings.find(name);
    if (pair != bindings.end()) {
      return Result(
          EXCEPTION, new Exception("Variable already declared: " + name));
    }
    bindings[name] = value;
    return Result(NORMAL, value);
  }
};

//** ast

struct Ast {
  Token *const token;
  Ast(Token *t): token(t) {}
  virtual Result eval(Scope *scope) const noexcept=0;
};

struct Literal final: Ast {
  Reference value;
  Literal(Token *t, Reference v): Ast(t), value(v) {}
  Result eval(Scope *scope) const noexcept {
    return Result(NORMAL, value);
  }
};

struct Name final: Ast {
  const std::string name;
  Name(Token *t, const std::string& n): Ast(t), name(n) {}
  Result eval(Scope *scope) const noexcept { return scope->get(name); }
};

}

#endif//rock_hh
