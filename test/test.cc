// TODO: Factor out tests

#include "rock/all.hh"
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
    Unit *unit = parseFile("<test>", "a + b");
    assert(unit->node->debug() == "Block(N4rock10MethodCallE,)");
    Scope scope;
    assert(
        unit->node->eval(scope).debug() ==
        "Result(EXCEPTION,<Exception: No such variable (get): 'a'>)");
  }
  {
    Unit *unit = parseFile("<test>", "String");
    assert(unit->node->debug() == "Block(N4rock4NameE,)");
    Scope scope(builtins);
    assert(unit->node->eval(scope).debug() == "Result(OK,<Class String>)");
  }
  {
    Unit *unit = parseFile("<test>", "'hello world!'");
    assert(unit->node->debug() == "Block(N4rock7LiteralE,)");
    Scope scope(builtins);
    assert(
        unit->node->eval(scope).debug() ==
        "Result(OK,String(hello world!))");
  }

  {
    Unit *unit = parseFile("<test>", "L(1.0,2,3).__str()");
    assert(unit->node->debug() == "Block(N4rock10MethodCallE,)");
    Scope scope(builtins);
    Result result = unit->node->eval(scope);
    assert(result.type == Result::Type::OK);
    assert(result.value->str() == "L(1, 2, 3)");
  }
  rock::finalize();
}
