#ifndef rock_hh
#define rock_hh

#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
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
struct NativeFunction;

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
extern Class *classNumber;
extern Class *classString;
extern Class *classNativeFunction;

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
  virtual Class *getClass()=0;
};

struct Class final: Value {
  std::map<std::string,Method> method_table;
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

struct Number final: Value {
  const double value;
  Number(double v): value(v) {}
  Class *getClass() override { return classNumber; }
};

struct String final: Value {
  const std::string value;
  String(const std::string& s): value(s) {}
  Class *getClass() override { return classString; }
};

struct NativeFunction final: Value {
  const std::function<Result(const Args&)> function;
  NativeFunction(std::function<Result(const Args&)> f): function(f) {}
  Class *getClass() override { return classNativeFunction; }
};

//** scope

struct Scope final {
  Scope *const parent;
  std::map<std::string, Reference> bindings;
  Scope(): Scope(nullptr) {}
  Scope(Scope *p): parent(p) {}
  Result get(const std::string& name) const {
    auto pair = bindings.find(name);
    if (pair != bindings.end()) {
      return Result(NORMAL, pair->second);
    }
    if (!parent) {
      return Result(EXCEPTION, new Exception("No such variable: " + name));
    }
    return parent->get(name);
  }
  Result set(const std::string& name, Reference value) {
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
  Result declare(const std::string& name, Reference value) {
    auto pair = bindings.find(name);
    if (pair != bindings.end()) {
      return Result(
          EXCEPTION, new Exception("Variable already declared: " + name));
    }
    bindings[name] = value;
    return Result(NORMAL, value);
  }
};

Scope *makeGlobalScope();

//** ast

struct Ast {
  Token *const token;
  Ast(Token *t): token(t) {}
  virtual Result eval(Scope *scope) const=0;
  virtual bool isError() const { return false; }
};

struct ParseError final: Ast {
  const std::string message;
  ParseError(Token *t, const std::string& m): Ast(t), message(m) {}
  Result eval(Scope *scope) const override { return Result(EXCEPTION, nil); }
  bool isError() const override { return true; }
};

struct Module final: Ast {
  const std::vector<Ast*> expressions;
  Module(Token *t, const std::vector<Ast*>& es):
      Ast(t), expressions(es) {}
  Result eval(Scope *scope) const {
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
  Block(Token *t, const std::vector<Ast*>& es):
      Ast(t), expressions(es) {}
  Result eval(Scope *parentScope) const {
    Scope scope(parentScope);
    Reference last(nil);
    for (Ast *e: expressions) {
      Result result = e->eval(&scope);
      if (result.type != NORMAL) {
        return result;
      }
      last = result.value;
    }
    return Result(NORMAL, last);
  }
};

struct Literal final: Ast {
  Reference value;
  Literal(Token *t, Reference v): Ast(t), value(v) {}
  Result eval(Scope *scope) const {
    return Result(NORMAL, value);
  }
};

struct Name final: Ast {
  const std::string name;
  Name(Token *t, const std::string& n): Ast(t), name(n) {}
  Result eval(Scope *scope) const { return scope->get(name); }
};

struct Declare final: Ast {
  const std::string name;
  Ast *const value;
  Declare(Token *t, Ast *const v): Ast(t), value(v) {}
  Result eval(Scope *scope) const {
    Result r = value->eval(scope);
    return r.type == NORMAL ? scope->declare(name, r.value) : r;
  }
};

struct Assign final: Ast {
  const std::string name;
  Ast *const value;
  Assign(Token *t, Ast *const v): Ast(t), value(v) {}
  Result eval(Scope *scope) const {
    Result r = value->eval(scope);
    return r.type == NORMAL ? scope->set(name, r.value) : r;
  }
};

struct MethodCall final: Ast {
  Ast *owner;
  const std::string name;
  std::vector<Ast*> args;
  MethodCall(
      Token *t, Ast *onr, const std::string& n, const std::vector<Ast*>& aa):
      Ast(t), owner(onr), name(n), args(aa) {}
  Result eval(Scope *scope) const {
    std::vector<Reference> args;
    Result ownerResult = this->owner->eval(scope);
    if (ownerResult.type != NORMAL) { return ownerResult; }
    Reference owner = ownerResult.value;
    for (Ast *arg: this->args) {
      Result r = arg->eval(scope);
      if (r.type != NORMAL) { return r; }
      args.push_back(r.value);
    }
    return owner.call(name, args);
  }
};

//** lexer

struct Lexer final {
  File *const file;
  long position;
  Token *last;
  bool doneFlag;
  std::vector<std::string> parenthesisStack;
  Lexer(File *f):
      file(f), position(0), last(nullptr), doneFlag(false) {
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

struct MaybeExpressionList {
  const std::vector<Ast*> expressions;
  Ast *error;
  MaybeExpressionList(Ast *e): error(e) {}
  MaybeExpressionList(const std::vector<Ast*>& es):
      expressions(es), error(nullptr) {}
};

struct Parser final {
  File *const file;
  const std::vector<Token*> tokens;
  long position;
  Parser(File *f) : file(f), tokens(lex(f)), position(0) {}
  ParseError *error(Token *t, const std::string& message) {
    return new ParseError(t, message);
  }
  ParseError *error(const std::string& message) {
    return error(peek(), message);
  }
  Token *peek() const {
    return tokens[position];
  }
  Token *next() {
    Token *t = peek();
    position++;
    return t;
  }
  bool at(const std::string& type) const {
    return peek()->type == type;
  }
  bool consume(const std::string& type) {
    if (at(type)) {
      next();
      return true;
    }
    return false;
  }
  void consumeStatementDelimiters() {
    while (at("NEWLINE") || at(";")) {
      next();
    }
  }
  Ast *parseModule() {
    Token *const token = peek();
    std::vector<Ast*> exprs;
    consumeStatementDelimiters();
    while (!at("EOF")) {
      Ast *e = parseStatement();
      if (e->isError()) { return e; }
      exprs.push_back(e);
      consumeStatementDelimiters();
    }
    return new Module(token, exprs);
  }
  Ast *parseBlock() {
    Token *const token = peek();
    if (!at("{")) { return error("Expected open brace for Block"); } next();
    std::vector<Ast*> exprs;
    consumeStatementDelimiters();
    while (!consume("}")) {
      Ast *e = parseStatement();
      if (e->isError()) { return e; }
      exprs.push_back(e);
      consumeStatementDelimiters();
    }
    return new Block(token, exprs);
  }
  Ast *parseStatement() {
    Ast *e = parseExpression();
    // {
    if (!at("NEWLINE") && !at(";") && !at("}")) {
      return error(
          "Expected NEWLINE, ';' or close brace to delimit end of statement "
          "but found: " + peek()->type);
    }
    return e;
  }
  Ast *parseExpression() {
    return parsePostfixExpression();
  }
  MaybeExpressionList parseExpressionList(
      const std::string& open, const std::string& close) {
    if (!consume(open)) { return error("Expected " + open); }
    std::vector<Ast*> exprs;
    while (!consume(close)) {
      Ast *e = parseExpression();
      if (e->isError()) { return e; }
      exprs.push_back(e);
    }
    return exprs;
  }
  Ast *parsePostfixExpression() {
    Ast *e = parsePrimaryExpression();
    while (true) {
      Token *t = peek();
      if (at("(")) {  // )
        MaybeExpressionList mexprlist = parseExpressionList("(", ")");
        if (mexprlist.error) { return mexprlist.error; }
        e = new MethodCall(t, e, "__call", mexprlist.expressions);
      } else if (consume(".")) {
        if (!at("NAME")) { return error("Expected name"); }
        const std::string name = next()->value;
        if (at("(")) {  // )
          MaybeExpressionList mexprlist = parseExpressionList("(", ")");
          if (mexprlist.error) { return mexprlist.error; }
          e = new MethodCall(t, e, name, mexprlist.expressions);
        } else if (at("=")) {
          Ast *v = parseExpression();
          if (v->isError()) { return v; }
          e = new MethodCall(t, e, "__set_" + name, {v});
        } else {
          e = new MethodCall(t, e, "__get_" + name, {});
        }
      } else {
        break;
      }
    }
    return e;
  }
  Ast *parsePrimaryExpression() {
    if (consume("(")) {
      Ast *expr = parseExpression();
      if (expr->isError()) { return expr; }
      if (!consume(")")) { return error("Expected close parenthesis"); }
      return expr;
    }
    Token *t = peek();
    if (at("NUMBER")) {
      return new Literal(t, new Number(std::stod(next()->value)));
    }
    if (at("STRING")) {
      return new Literal(t, new String(next()->value));
    }
    if (at("NAME")) {
      return new Name(t, next()->value);
    }
    if (at("{")) {  // }
      return parseBlock();
    }
    return error("Expected expression but found: " + peek()->type);
  }
};

inline Ast *parse(const std::string& type, File *file) {
  Parser parser(file);
  if (type == "Module") {
    return parser.parseModule();
  } else if (type == "Expression") {
    return parser.parseExpression();
  } else {
    return parser.error("Unrecognized parse type: " + type);
  }
}

inline Ast *parse(File *file) {
  return parse("Module", file);
}

}

#endif//rock_hh
