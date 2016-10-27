#ifndef rock_parser_hh
#define rock_parser_hh

#include <string>

#include "rock/number.hh"
#include "rock/string.hh"
#include "rock/lexer.hh"

namespace rock {

struct MaybeExpressionList final {
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

#endif//rock_parser_hh
