// g++ foo.cc src/rock.cc --std=c++11 -Iinclude -Wall -Werror -Wpedantic && ./a.out
#include <iostream>
#include <cassert>
#include "rock.hh"
using namespace std;
using namespace rock;

int main() {
  Ast *literal = new Literal(nullptr, new String("Foo"));
  Result result = literal->eval(nullptr);
  assert(result.type == NORMAL);

  {
    std::vector<Token*> tokens =
        lex(new File("<test>", "'a' b class 4.1 3 ."));
    assert(tokens.size() == 8);
    assert(tokens[0]->type == "STRING");
    assert(tokens[0]->value == "a");
    assert(tokens[1]->type == "NAME");
    assert(tokens[1]->value == "b");
    assert(tokens[2]->type == "class");
    assert(tokens[2]->value == "");
    assert(tokens[3]->type == "NUMBER");
    assert(tokens[3]->value == "4.1");
    assert(tokens[4]->type == "NUMBER");
    assert(tokens[4]->value == "3");
    assert(tokens[5]->type == ".");
    assert(tokens[5]->value == "");
    assert(tokens[6]->type == "NEWLINE");
    assert(tokens[6]->value == "");
    assert(tokens[7]->type == "EOF");
    assert(tokens[7]->value == "");
  }

  {
    // Don't skip newlines when inside {}
    std::vector<Token*> tokens = lex(new File("<test>", "{\n}"));
    assert(tokens.size() == 5);
    assert(tokens[0]->type == "{");
    assert(tokens[1]->type == "NEWLINE");
    assert(tokens[2]->type == "}");
    assert(tokens[3]->type == "NEWLINE");
    assert(tokens[4]->type == "EOF");
  }

  {
    // Skip newlines when inside ()
    std::vector<Token*> tokens = lex(new File("<test>", "(\n)"));
    assert(tokens.size() == 4);
    assert(tokens[0]->type == "(");
    assert(tokens[1]->type == ")");
    assert(tokens[2]->type == "NEWLINE");
    assert(tokens[3]->type == "EOF");
  }

  {
    File file("<test>", "'Hello world!'");
    Ast *a = parse("Expression", &file);
    assert(a);
    assert(!a->isError());
    Literal *lit = dynamic_cast<Literal*>(a);
    assert(lit);
    String *str = dynamic_cast<String*>(lit->value.pointer);
    assert(str);
    assert(str->value == "Hello world!");

    Scope scope;
    Result result = a->eval(&scope);
    assert(result.type == NORMAL);
    assert(result.value.pointer == str);
  }

  cout << "Tests pass!" << endl;
}
