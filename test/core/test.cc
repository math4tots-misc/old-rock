// TODO: Factor out tests

#include "rock/core/all.hh"
#include <iostream>
#include <memory>
#include <cassert>
using namespace std;
using namespace rock;

int main() {
  rock::initialize();

  assert(classClass->debug() == "<Class Class>");
  assert(classException->debug() == "<Class Exception>");

  File f("a", "b");
  File g(f);
  {
    Ast *node = parseFile("<test>", "a + b");
    assert(node->debug() == "Block(N4rock10MethodCallE,)");
    Scope scope;
    assert(
        node->eval(scope).debug() ==
        "Result(EXCEPTION,<Exception: No such variable (get): 'a'>)");
  }
  {
    Ast *node = parseFile("<test>", "String");
    assert(node->debug() == "Block(N4rock4NameE,)");
    Scope scope(builtins);
    assert(node->eval(scope).debug() == "Result(OK,<Class String>)");
  }
  {
    Ast *node = parseFile("<test>", "'hello world!'");
    assert(node->debug() == "Block(N4rock7LiteralE,)");
    Scope scope(builtins);
    assert(
        node->eval(scope).debug() ==
        "Result(OK,String(hello world!))");
  }

  {
    Ast *node = parseFile("<test>", "L(1.0,2,3).__str()");
    assert(node->debug() == "Block(N4rock10MethodCallE,)");
    Scope scope(builtins);
    Result result = node->eval(scope);
    assert(result.type == Result::Type::OK);
    assert(result.value->str() == "L(1, 2, 3)");
  }
  rock::finalize();
}
