#include "rock/core/all.hh"

namespace rock {

namespace {
// TODO: Rename snake_case method names to camelCase to be
// consistent with the rest of the codebase.
class Parser final {
  std::vector<Token> tokens;
  int i;
public:
  Parser(const File &f) {
    tokens = lex(f);
    if (tokens.back().type == "ERROR") {
      throw ParseError(tokens.back(), "Lex error: " + tokens.back().value);
    }
    i = 0;
  }

  Token peek() const {
    return tokens[i];
  }

  Token gettok() {
    return tokens[i++];
  }

  bool at(std::string type) const {
    return peek().type == type;
  }

  bool consume(std::string type) {
    if (at(type)) {
      gettok();
      return true;
    }
    return false;
  }

  Token expect(std::string type) {
    if (peek().type != type) {
      Token token(peek());
      std::string msg = "Expected " + type + " but got " + peek().type;
      if (!peek().value.empty()) {
        msg += " (" + peek().value + ")";
      }
      throw ParseError(token, msg);
    }
    return gettok();
  }

  void consumeStatementDelimiters() {
    while (consume(";") || consume("NEWLINE"));
  }

  //////////////

  Block *parse_module() {
    Token t = peek();
    std::vector<Ast*> stmts;
    consumeStatementDelimiters();
    while (!at("EOF")) {
      stmts.push_back(parse_expression());
      consumeStatementDelimiters();
    }
    return new Block(t, stmts);
  }

  If *parse_if() {
    Token t = expect("if");
    Ast *cond = parse_expression();
    // If you see a then clause, you don't need each body to be a block
    bool then = consume("then");
    Ast *body = then ? parse_expression() : parse_block();
    Ast *other = nullptr;
    if (consume("else")) {
      if (then) {
        other = parse_expression();
      } else {
        if (at("if")) {
          other = parse_if();
        } else {
          other = parse_block();
        }
      }
    }
    return new If(t, cond, body, other);
  }

  Block *parse_block() {
    Token t = expect("{");
    std::vector<Ast*> stmts;
    consumeStatementDelimiters();
    while (!consume("}")) {
      stmts.push_back(parse_expression());
      consumeStatementDelimiters();
    }
    return new Block(t, stmts);
  }

  Ast *parse_expression() {
    return parse_or_expression();
  }

  Ast *parse_or_expression() {
    Ast *e = parse_and_expression();
    for (;;) {
      Token t = peek();
      if (consume("or")) {
        e = new Or(t, e, parse_and_expression());
      }
      break;
    }
    return e;
  }

  Ast *parse_and_expression() {
    Ast *e = parse_relational_expression();
    for (;;) {
      Token t = peek();
      if (consume("and")) {
        e = new And(t, e, parse_relational_expression());
      }
      break;
    }
    return e;
  }

  // TODO: Treat this the way Python does it.
  Ast *parse_relational_expression() {
    Ast *e = parse_additive_expression();
    Token t = peek();
    if (consume("==")) {
      return new MethodCall(
          t, e, "__eq", new Arguments(t, {parse_additive_expression()}));
    }
    if (consume("!=")) {
      return new MethodCall(
          t, e, "__ne", new Arguments(t, {parse_additive_expression()}));
    }
    if (consume("<")) {
      return new MethodCall(
          t, e, "__lt", new Arguments(t, {parse_additive_expression()}));
    }
    if (consume("<=")) {
      return new MethodCall(
          t, e, "__le", new Arguments(t, {parse_additive_expression()}));
    }
    if (consume(">")) {
      return new MethodCall(
          t, e, "__gt", new Arguments(t, {parse_additive_expression()}));
    }
    if (consume(">=")) {
      return new MethodCall(
          t, e, "__ge", new Arguments(t, {parse_additive_expression()}));
    }
    if (consume("is")) {
      if (consume("not")) {
        return new MethodCall(
            t, e, "__is_not",
            new Arguments(t, {parse_additive_expression()}));
      } else {
        return new MethodCall(
            t, e, "__is", new Arguments(t, {parse_additive_expression()}));
      }
    }
    return e;
  }

  Ast *parse_additive_expression() {
    Ast *e = parse_multiplicative_expression();
    for (;;) {
      Token t = peek();
      if (consume("+")) {
        e = new MethodCall(
            t, e, "__add",
            new Arguments(t, {parse_multiplicative_expression()}));
        continue;
      }
      if (consume("-")) {
        e = new MethodCall(
            t, e, "__sub",
            new Arguments(t, {parse_multiplicative_expression()}));
        continue;
      }
      break;
    }
    return e;
  }

  Ast *parse_multiplicative_expression() {
    Ast *e = parse_prefix_expression();
    for (;;) {
      Token t = peek();
      if (consume("*")) {
        e = new MethodCall(
            t, e, "__mul", new Arguments(t, {parse_prefix_expression()}));
        continue;
      }
      if (consume("/")) {
        e = new MethodCall(
            t, e, "__div", new Arguments(t, {parse_prefix_expression()}));
        continue;
      }
      if (consume("%")) {
        e = new MethodCall(
            t, e, "__mod", new Arguments(t, {parse_prefix_expression()}));
        continue;
      }
      break;
    }
    return e;
  }

  Ast *parse_prefix_expression() {
    Token t = peek();
    if (consume("-")) {
      return new MethodCall(
          t, parse_prefix_expression(), "__neg", new Arguments(t, {}));
    }
    if (consume("not")) {
      return new MethodCall(
          t, parse_prefix_expression(), "__not", new Arguments(t, {}));
    }
    return parse_postfix_expression();
  }

  Ast *parse_postfix_expression() {
    Ast *e = parse_primary_expression();
    for (;;) {
      Token t = peek();
      if (at("(")) {  // )
        e = new MethodCall(t, e, "__call", parse_arguments());
        continue;
      } else if (consume(".")) {
        std::string name = expect("ID").value;
        if (consume("=")) {
          e = new MethodCall(
              t, e, "__set_" + name, new Arguments(t, {parse_expression()}));
        } else if (at("(")) {  // )
          e = new MethodCall(t, e, name, parse_arguments());
        } else {
          e = new MethodCall(t, e, "__get_" + name, new Arguments(t, {}));
        }
        continue;
      }
      break;
    }
    return e;
  }

  Arguments *parse_arguments() {
    Token t(peek());
    std::vector<Ast*> argexprs;
    Ast *varargexpr = nullptr;
    expect("(");
    while (!consume(")")) {
      if (consume("*")) {
        varargexpr = parse_expression();  // (
        expect(")");
        break;
      }
      argexprs.push_back(parse_expression());
      consume(",");
    }
    return new Arguments(t, argexprs, varargexpr);
  }

  Ast *parse_primary_expression() {
    Token t = peek();
    if (at("{")) {  // }
      return parse_block();
    } else if (consume("while")) {
      Ast *cond = parse_expression();
      Block *body = parse_block();
      return new While(t, cond, body);
    } else if (at("if")) {
      return parse_if();
    } else if (consume("(")) {
      Ast *e = parse_expression();
      expect(")");
      return e;
    } else if (at("ID")) {
      std::string name = expect("ID").value;
      if (consume("=")) {
        Ast *e = parse_expression();
        return new Assignment(t, name, e);
      } else {
        return new Name(t, name);
      }
    } else if (consume("var")) {
      std::string name = expect("ID").value;
      if (consume("=")) {
        Ast *e = parse_expression();
        return new Declaration(t, name, e);
      } else {
        return new Declaration(t, name, nullptr);
      }
    } else if (at("NUM")) {
      return new Literal(t, new Number(stod(expect("NUM").value)));
    } else if (at("STR")) {
      return new Literal(t, String::from(expect("STR").value));
    } else if (consume("class")) {
      std::string name;
      if (at("ID")) {
        name = expect("ID").value;
      } else if (at("STR")) {
        name = expect("STR").value;
      }
      Arguments *bases;
      if (at("(")) {  // )
        bases = parse_arguments();
      } else {
        bases = new Arguments(t, {new Name(t, "Object")});
      }
      std::set<std::string> fields;
      std::map<std::string,FunctionDisplay*> methods;
      expect("{");
      consumeStatementDelimiters();
      while (!consume("}")) {
        if (consume("var")) {
          // TODO: detect duplicates
          fields.insert(expect("ID").value);
          while (consume(",")) {
            fields.insert(expect("ID").value);
          }
        } else if (at("def")) {
          Ast *expr = parse_primary_expression();
          FunctionDisplay *method =
              dynamic_cast<FunctionDisplay*>(expr);
          if (!method) {
            throw ParseError(
                t,
                "FUBAR: Internal consistency error... "
                "Thought there was a method here, but found "
                "something else: " + expr->debug());
          }
          methods[method->name] = method;
        } else {
          throw ParseError(
              peek(), "Expected field or method declaration " + peek().type);
        }
        consumeStatementDelimiters();
      }
      return new ClassDisplay(t, name, bases, fields, methods);
    } else if (consume("def")) {
      std::string name;
      if (at("ID")) {
        name = expect("ID").value;
      } else if (at("STR")) {
        name = expect("STR").value;
      }
      Signature *sig = parse_signature_with_parenthesis();
      Ast *body =
          consume("=") ?
          (consumeStatementDelimiters(), parse_expression()) :
          parse_block();
      return new FunctionDisplay(t, name, sig, body);
    } else if (consume("\\")) {
      Signature *sig = parse_inside_signature();
      Ast *body;
      if (consume(".")) {
        body = parse_expression();
      } else {
        body = parse_block();
      }
      return new FunctionDisplay(t, "", sig, body);
    } else if (consume("super")) {
      expect(".");
      std::string name = expect("ID").value;
      return new SuperCall(t, name, parse_arguments());
    } else {
      throw ParseError(
          peek(), "Expected expression but found " + peek().type);
    }
  }

  Signature *parse_signature_with_parenthesis() {
    expect("(");
    Signature *sig = parse_inside_signature();
    expect(")");
    return sig;
  }

  Signature *parse_inside_signature() {
    Token t(peek());
    std::vector<std::string> argnames;
    std::vector<std::string> optargnames;
    std::string varargname;
    while (at("ID") || at("*") || at("/")) {
      if (consume("*")) {
        varargname = expect("ID").value;
        break;
      }
      if (consume("/")) {
        optargnames.push_back(expect("ID").value);
      } else {
        argnames.push_back(expect("ID").value);
      }
      consume(",");
    }
    return new Signature(t, argnames, optargnames, varargname);
  }
};
}  // namespace

Block *parseFile(File *f) {
  Parser parser(*f);
  return parser.parse_module();
}

Block *parseFile(const std::string &f, const std::string &c) {
  return parseFile(new File(f, c));
}

Block *parseModule(const std::string &c) {
  return parseFile(new File("<parseModule>", c));
}

Ast *parseExpression(const std::string &c) {
  File *f = new File("<parseExpression>", c);
  Parser parser(*f);
  return parser.parse_expression();
}

}  // namespace rock
