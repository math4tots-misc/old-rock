// TODO: Factor out tests

#include "rock/all.hh"
#include <iostream>
#include <memory>
#include <cassert>
using namespace std;
using namespace rock;

int main() {
  rock::initialize();

  {
    // lex test
    File f("<lex-test>", "a 'b' 3 4.4 +");
    vector<Token> tokens = lex(f);
    assert(tokens.size() == 6);
    assert(tokens[0].type == "ID");
    assert(tokens[0].value == "a");
  }

  cout << classClass->debug() << endl;
  cout << classException->debug() << endl;

  File f("a", "b");
  File g(f);
  {
    std::unique_ptr<Unit> unit = parseFile("<test>", "a + b");
    cout << unit->node->debug() << endl;
    Scope scope;
    cout << unit->node->eval(scope).debug() << endl;
  }
  {
    std::unique_ptr<Unit> unit = parseFile("<test>", "String");
    cout << unit->node->debug() << endl;
    Scope scope(builtins);
    cout << unit->node->eval(scope).debug() << endl;
  }
  {
    std::unique_ptr<Unit> unit = parseFile("<test>", "print('hello world!')");
    cout << unit->node->debug() << endl;
    Scope scope(builtins);
    cout << unit->node->eval(scope).debug() << endl;
  }

  {
    std::unique_ptr<Unit> unit = parseFile("<test>", "L(1.0,2,3).__str()");
    cout << unit->node->debug() << endl;
    Scope scope(builtins);
    Result result = unit->node->eval(scope);
    assert(result.type == Result::Type::OK);
    assert(result.value->str() == "L(1, 2, 3)");
  }
  rock::finalize();
}
