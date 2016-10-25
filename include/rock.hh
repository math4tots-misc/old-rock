#ifndef rock_hh
#define rock_hh

#include <algorithm>
#include <cctype>
#include <functional>
#include <map>
#include <set>
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
  const std::string type, value;
  Token(File *const f, long p, const std::string& t): Token(f, p, t, "") {}
  Token(File *const f, long p, const std::string& t, const std::string& v):
      file(f), position(p), type(t), value(v) {}
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

struct Module final: Ast {
  const std::vector<Ast*> expressions;
  Module(Token *t, const std::vector<Ast*>& es): Ast(t), expressions(es) {}
  Result eval(Scope *scope) const noexcept {
    for (Ast *e: expressions) {
      Result result = e->eval(scope);
      if (result.type != NORMAL) {
        return result;
      }
    }
    return Result(NORMAL, nil);
  }
};

struct Block final: Ast {
  const std::vector<Ast*> expressions;
  Block(Token *t, const std::vector<Ast*>& es): Ast(t), expressions(es) {}
  Result eval(Scope *parentScope) const noexcept {
    Scope scope(parentScope);
    Reference last(nil);
    for (Ast *e: expressions) {
      Result result = e->eval(&scope);
      if (result.type != NORMAL) {
        return result;
      }
    }
    return Result(NORMAL, last);
  }
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

struct Declare final: Ast {
  const std::string name;
  Ast *const value;
  Declare(Token *t, Ast *const v): Ast(t), value(v) {}
  Result eval(Scope *scope) const noexcept {
    Result r = value->eval(scope);
    return r.type == NORMAL ? scope->declare(name, r.value) : r;
  }
};

struct Assign final: Ast {
  const std::string name;
  Ast *const value;
  Assign(Token *t, Ast *const v): Ast(t), value(v) {}
  Result eval(Scope *scope) const noexcept {
    Result r = value->eval(scope);
    return r.type == NORMAL ? scope->set(name, r.value) : r;
  }
};

//** lexer

struct Lexer final {
  File *const file;
  long position;
  Token *last;
  bool doneFlag;
  std::vector<std::string> parenthesisStack;
  Lexer(File *f): file(f), position(0), last(nullptr), doneFlag(false) {
    last = extract();
  }

  Token *peek() const {
    return last;
  }

  Token *next() {
    if (last->type == "ERR" || last->type == "EOF") {
      doneFlag = true;
      return last;
    } else {
      Token *token = last;
      last = extract();
      return token;
    }
  }

  bool done() const {
    return doneFlag;
  }

private:
  static bool isword(char c) {
    return c == '_' || std::isalnum(c);
  }
  std::string slice(long start, long end) const {
    return file->contents.substr(start, end - start);
  }
  char ch() const {
    return ch(0);
  }
  char ch(long diff) const {
    long p = position + diff;
    return (p < 0 || p >= file->contents.size()) ? '\0' : file->contents[p];
  }
  bool startsWith(const std::string& prefix) const {
    return startsWith(prefix, 0);
  }
  bool startsWith(const std::string& prefix, long diff) const {
    for (char c: prefix) {
      if (ch(diff++) != c) {
        return false;
      }
    }
    return true;
  }
  // The rule for newline tokens:
  // If the innermost grouping is '()' or '[]', newlines are ignored.
  // If you are at the top level, or the innermost grouping is '{}',
  // newline tokens are generated.
  bool isSpace(char c) const {
    return c == '\n' ?
           !parenthesisStack.empty() && (
              parenthesisStack.back() == "(" ||  // )
              parenthesisStack.back() == "[") :  // ]
           std::isspace(c);
  }
  void skipWhitespace() {
    while (isSpace(ch())) {
      position++;
    }
  }
  Token *extract() {
    std::set<std::string> keywords({
      "class", "for", "return",
    });
    std::vector<std::string> symbols({
      "(", ")", "[", "]", "{", "}", ";", "\\", ".",
    });
    std::map<std::string, std::string> matches({
      {"(", ")"},
      {"[", "]"},
      {"{", "}"},
    });
    std::set<std::string> openers({"(", "[", "{"});
    std::set<std::string> closers({")", "]", "}"});
    std::sort(symbols.begin(), symbols.end());
    std::reverse(symbols.begin(), symbols.end());

    skipWhitespace();

    if (ch() == '\0') {
      if (!parenthesisStack.empty()) {
        return new Token(
            file, position, "ERR",
            std::string("Mismatched parenthesis: ") +
                parenthesisStack.back());
      }
      if (last && last->type == "NEWLINE") {
        return new Token(file, position, "EOF");
      } else {
        return new Token(file, position, "NEWLINE");
      }
    }

    const long p = position;

    // NEWLINE
    if (ch() == '\n') {
      position++;
      return new Token(file, p, "NEWLINE");
    }

    // STRING
    if (startsWith("r'") || startsWith("'") ||
        startsWith("r\"") || startsWith("\"")) {
      bool raw = false;
      if (startsWith("r")) {
        raw = true;
        position++;
      }
      std::string quote = slice(position, position+1);
      if (startsWith(quote + quote + quote)) {
        quote = slice(position, position + 3);
      }
      position += quote.size();
      std::string value;
      while (!startsWith(quote)) {
        if (ch() == '\0') {
          return new Token(file, position, "ERR", "Unfinished string");
        }
        if (!raw && ch() == '\\') {
          position++;
          switch (ch()) {
          case 'n': value += '\n'; break;
          case 't': value += '\t'; break;
          case '\'': value += '\''; break;
          case '\"': value += '\"'; break;
          default:
            return new Token(file, position, "ERR", "Invalid escape");
          }
          position++;
        } else {
          value += ch();
          position++;
        }
      }
      position += quote.size();
      return new Token(file, p, "STRING", value);
    }

    // SYMBOL
    for (const auto& symbol: symbols) {
      if (startsWith(symbol)) {
        if (openers.find(symbol) != openers.end()) {
          parenthesisStack.push_back(symbol);
        } else if (closers.find(symbol) != closers.end()) {
          if (matches[parenthesisStack.back()] != symbol) {
            return new Token(file, p, "ERR", "Mismatched parenthesis");
          } else {
            parenthesisStack.pop_back();
          }
        }
        position += symbol.size();
        return new Token(file, p, symbol);
      }
    }

    // NUMBER
    if (std::isdigit(ch()) || (ch() == '.' && std::isdigit(ch(1)))) {
      while (std::isdigit(ch())) {
        position++;
      }
      if (ch() == '.') {
        position++;
        while (std::isdigit(ch())) {
          position++;
        }
      }
      return new Token(file, p, "NUMBER", slice(p, position));
    }

    // NAME/keywords
    while (isword(ch())) {
      position++;
    }
    if (p != position) {
      auto word = slice(p, position);
      if (keywords.find(word) == keywords.end()) {
        return new Token(file, p, "NAME", word);
      } else {
        return new Token(file, p, word);
      }
    }

    // Unrecognized token
    return new Token(file, position, "ERR", "Unrecognized token");
  }
};

inline std::vector<Token*> lex(File *f) {
  std::vector<Token*> tokens;
  Lexer lexer(f);
  while (!lexer.done()) {
    tokens.push_back(lexer.next());
  }
  return tokens;
}

//** parser

struct Parser final {
  File *const file;
  long position;
  Parser(File *f): file(f), position(0) {}
  Ast *parseExpression() {
    return nullptr;
  }
};

}

#endif//rock_hh
